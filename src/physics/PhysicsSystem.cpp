#include "physics/PhysicsSystem.h"
#include "transform/Transform.h"

using namespace sitara::ecs;

PhysicsSystem::PhysicsSystem() {
	mFoundation = nullptr;
	mPhysics = nullptr;
	mDispatcher = nullptr;
	mCudaContext = nullptr;
	mScene = nullptr;
	mPvd = nullptr;
	mNumberOfThreads = 8;
	mMaterialCount = -1;
	mSimulationTime = 0.0f;
	mGpuEnabled = false;
}


PhysicsSystem::~PhysicsSystem() {
	if (mScene) {
		mScene->release();
		mScene = nullptr;
	}
	if (mDispatcher) {
		mDispatcher->release();
		mDispatcher = nullptr;
	}
	if (mCudaContext) {
		mCudaContext->release();
		mCudaContext = nullptr;
	}
	if (mPhysics) {
		mPhysics->release();
		mPhysics = nullptr;
	}
	if (mPvd) {
		physx::PxPvdTransport* transport = mPvd->getTransport();
		mPvd->release();
		mPvd = nullptr;
		if (transport) {
			transport->release();
			transport = nullptr;
		}
	}
	if (mFoundation) {
		mFoundation->release();
		mFoundation = nullptr;
	}
}

void PhysicsSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
	mEntities = &entities;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(mNumberOfThreads);

	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd);
	PxInitExtensions(*mPhysics, mPvd);

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//sceneDesc.flags = physx::PxSceneFlag::eREQUIRE_RW_LOCK;

	if (mGpuEnabled) {
		physx::PxCudaContextManagerDesc cudaDesc;
		mCudaContext = PxCreateCudaContextManager(*mFoundation, cudaDesc);
		sceneDesc.cudaContextManager = mCudaContext;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
	}

	mScene = mPhysics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	events.subscribe<entityx::ComponentAddedEvent<DynamicBody>>(*this);
	events.subscribe<entityx::ComponentRemovedEvent<DynamicBody>>(*this);
	events.subscribe<entityx::ComponentAddedEvent<StaticBody>>(*this);
	events.subscribe<entityx::ComponentRemovedEvent<StaticBody>>(*this);
}

void PhysicsSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::StaticBody> sBody;
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body;
	entityx::ComponentHandle<sitara::ecs::OverlapDetector> overlapDetector;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;

	// preprocessing callbacks
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mPreUpdateFns) {
			callback(body);
		}
	}

	// run simulation
	float timeStep = static_cast<float>(dt);
	mSimulationTime += timeStep;
	mScene->simulate(timeStep);
	mScene->fetchResults(true);

	// update transform component with new world transform from physx
	for (auto entity : entities.entities_with_components(body, transform)) {
		if (!body->isSleeping()) {
			transform->mPosition = body->getPosition();
			transform->mOrientation = body->getRotation();
		}
	}

	for (auto entity : entities.entities_with_components(sBody, transform)) {
		if (sBody->isDirty()) {
			transform->mPosition = sBody->getPosition();
			transform->mOrientation = sBody->getRotation();
			sBody->setDirty(false);
		}
	}

	/*
	* Iterate over proximity detector components and detect proximities
	* Any PhysX Scene Queries should go here
	*/
	for (auto entity : entities.entities_with_components(overlapDetector, transform)) {
		overlapDetector->setTransform(sitara::ecs::physics::to(transform->mOrientation, transform->mPosition));

		size_t num = physx::PxSceneQueryExt::overlapMultiple(*mScene,
			overlapDetector->getGeometry(),
			overlapDetector->getTransform(),
			overlapDetector->getWriteBuffer(),
			overlapDetector->getBufferSize(),
			overlapDetector->getFilter());

		overlapDetector->resizeBuffer(num);

		for (auto& hit : overlapDetector->getResults()) {
			if (hit.actor != nullptr) {
				auto previous = overlapDetector->getPreviousResults();
				auto it = std::find_if(previous.begin(), previous.end(), [&](const physx::PxOverlapHit& h) { return hit.actor == h.actor; });
				if (it != previous.end()) {
					// in current collision + previous collision, still colliding
					entityx::Entity e = entity;
					entityx::Entity overlappingEntity = mEntities->get(entityx::Entity::Id((uint64_t)(hit.actor->userData)));
					if (e.id().id() != overlappingEntity.id().id()) {
						for (auto& fn : overlapDetector->mDuringEachOverlapFns) {
							fn(e, overlappingEntity);
						}
					}
				}
				else {
					// in current collision but not previous collision, started colliding
					entityx::Entity e = entity;
					entityx::Entity overlappingEntity = mEntities->get(entityx::Entity::Id((uint64_t)(hit.actor->userData)));
					if (e.id().id() != overlappingEntity.id().id()) {
						for (auto& fn : overlapDetector->mOnEnterEachOverlapFns) {
							fn(e, overlappingEntity);
						}
					}
				}
			}
		}

		for (auto& hit : overlapDetector->getPreviousResults()) {
			if (hit.actor != nullptr) {
				auto results = overlapDetector->getResults();
				auto it = std::find_if(results.begin(), results.end(), [&](const physx::PxOverlapHit& h) { return hit.actor == h.actor; });
				if (it == results.end()) {
					// in previous collision but NOT in current collision, ending collision
					entityx::Entity e = entity;
					entityx::Entity overlappingEntity = mEntities->get(entityx::Entity::Id((uint64_t)(hit.actor->userData)));
					if (e.id().id() != overlappingEntity.id().id()) {
						for (auto& fn : overlapDetector->mOnEndEachOverlapFns) {
							fn(e, overlappingEntity);
						}
					}
				}
			}
		}

		overlapDetector->saveResults();
	}

	// post processing
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mPostUpdateFns) {
			callback(body);
		}
	}
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::DynamicBody>& event) {
	/*
	Link the user application entityx::Entity::Id to the NVIDIA PhysX object using the userData pointer.
	Note that this is NOT a pointer to the entity; it's the entity's literal id number.
	use ->getUserData() to get the entity's ID and mEntities.get(id); to get the entity handle directly
	*/
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body = event.component;
	body->mBody->userData = (void*)(event.entity.id().id());
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::DynamicBody>& event) {
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::StaticBody>& event) {
	/*
	Link the user application entityx::Entity::Id to the NVIDIA PhysX object using the userData pointer.
	Note that this is NOT a pointer to the entity; it's the entity's literal id number.
	use ->getUserData() to get the entity's ID and mEntities.get(id); to get the entity handle directly
	*/
	entityx::ComponentHandle<sitara::ecs::StaticBody> body = event.component;
	body->mBody->userData = (void*)(event.entity.id().id());
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::StaticBody>& event) {
}

double PhysicsSystem::getElapsedSimulationTime() {
	return mSimulationTime;
}

void PhysicsSystem::setGravity(const ci::vec3& gravity) {
	if (mScene) {
		mScene->setGravity(physx::PxVec3(gravity.x, gravity.y, gravity.z));
	}
	else {
		std::cout << "sitara::ecs::PhysicsSystem ERROR -- must configure() system before you can set gravity." << std::endl;
	}
}

void PhysicsSystem::setNumberOfThreads(const uint32_t numThreads) {
	if (!mScene) {
		mNumberOfThreads = numThreads;
	}
	else {
		std::cout << "Number of threads must be set before running PhysicsSystem::configure(); using default number of threads : " << mNumberOfThreads << std::endl;
	}
}

void PhysicsSystem::enableGpu(const bool enable) {
	if (!mScene) {
		std::cout << "GPU features are currently only available for dynamic builds; until a future time these features are disabled." << std::endl;
		mGpuEnabled = true;
	}
	else {
		std::cout << "GPU must be enabled before running PhysicsSystem::configure(); using CPU dispatcher instead." << std::endl;
	}
}

physx::PxRigidStatic* PhysicsSystem::createStaticBody(const ci::vec3& position, const ci::quat& rotation) {
	physx::PxTransform transform = sitara::ecs::physics::to(rotation, position);
	physx::PxRigidStatic* body = mPhysics->createRigidStatic(transform);
	mScene->addActor(*body);
	return body;
}

physx::PxRigidDynamic* PhysicsSystem::createDynamicBody(const ci::vec3& position, const ci::quat& rotation) {
	physx::PxTransform transform = sitara::ecs::physics::to(rotation, position);
	physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(transform);
	mScene->addActor(*body);
	return body;
}

physx::PxDistanceJoint* PhysicsSystem::createSpring(entityx::ComponentHandle<sitara::ecs::DynamicBody> body, ci::vec3 anchorPoint, float stiffness, float dampingConstant) {
	auto staticAnchor = createStaticBody(anchorPoint, ci::quat(0, ci::vec3(0, 0, 1)));

	auto springJoint = physx::PxDistanceJointCreate(*mPhysics,
		body->mBody,
		physx::PxTransform(physx::PxIdentity),
		staticAnchor,
		physx::PxTransform(physx::PxIdentity)
	);
	springJoint->setStiffness(stiffness);
	springJoint->setDamping(dampingConstant);
	springJoint->setDistanceJointFlags(physx::PxDistanceJointFlag::eSPRING_ENABLED);
	return springJoint;
}

int PhysicsSystem::registerMaterial(const float staticFriction = 0.5f, const float dynamicFriction = 0.5f, const float restitution = 0.0f) {
	mMaterialCount++;
	physx::PxMaterial* material = mPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
	mMaterialRegistry.insert(std::pair<int, physx::PxMaterial*>(mMaterialCount, material));
	return mMaterialCount;
}

physx::PxMaterial* PhysicsSystem::getMaterial(const int id) {
	return mMaterialRegistry[id];
}

void PhysicsSystem::addPreUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> callback) {
	mPreUpdateFns.push_back(callback);
}

void PhysicsSystem::addPostUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> callback) {
	mPostUpdateFns.push_back(callback);
}
