#include "physics/PhysicsSystem.h"
#include "transform/Transform.h"

using namespace sitara::ecs;

PhysicsSystem::PhysicsSystem() {
	mFoundation = NULL;
	mPhysics = NULL;
	mDispatcher = NULL;
	mScene = NULL;
	mPvd = NULL;
	mNumberOfThreads = 8;
	mMaterialCount = 0;
}


PhysicsSystem::~PhysicsSystem() {
	if (mScene) {
		mScene->release();
		mScene = NULL;
	}
	if (mDispatcher) {
		mDispatcher->release();
		mDispatcher = NULL;
	}
	if (mPhysics) {
		mPhysics->release();
		mPhysics = NULL;
	}
	if (mPvd) {
		physx::PxPvdTransport* transport = mPvd->getTransport();
		mPvd->release();
		mPvd = NULL;
		if (transport) {
			transport->release();
			transport = NULL;
		}
	}
	if (mFoundation) {
		mFoundation->release();
		mFoundation = NULL;
	}
}

void PhysicsSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(mNumberOfThreads);

	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd);

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//sceneDesc.flags = physx::PxSceneFlag::eREQUIRE_RW_LOCK;
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
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;

	/*
	// preprocessing callbacks
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mDynamicBodyPreUpdateFns) {
			callback(body);
		}
	}
	*/

	// run simulation
	float timeStep = static_cast<float>(dt);
	mScene->simulate(timeStep);
	mScene->fetchResults(true);

	// update transform component with new world transform from physx
	// only need to update dynamic bodies -- static bodies don't move!
	for (auto entity : entities.entities_with_components(body, transform)) {
		physx::PxTransform trans;

		transform->mPosition = body->getPosition();
		transform->mOrientation = body->getRotation();
	}

	/*
	// post processing
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mDynamicBodyPostUpdateFns) {
			callback(body);
		}
	}
	*/
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::DynamicBody>& event) {
	// set user data
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::DynamicBody>& event) {
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::StaticBody>& event) {
	// set user data
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::StaticBody>& event) {
}

double PhysicsSystem::getElapsedSimulationTime() {
	return mScene->getTimestamp();
}

void PhysicsSystem::setGravity(ci::vec3& gravity) {
	if (mScene) {
		mScene->setGravity(physx::PxVec3(gravity.x, gravity.y, gravity.z));
	}
	else {
		std::cout << "sitara::ecs::PhysicsSystem ERROR -- must configure() system before you can set gravity." << std::endl;
	}
}

void PhysicsSystem::setNumberOfThread(uint32_t numThreads) {
	if (!mScene) {
		mNumberOfThreads = numThreads;
	}
	else {
		std::cout << "Number of threads must be set before running PhysicsSystem::configure(); using default number of threads : " << mNumberOfThreads << std::endl;
	}
}

physx::PxRigidStatic* PhysicsSystem::createStaticBody(ci::vec3& position, ci::quat& rotation) {
	physx::PxTransform transform = sitara::ecs::physics::to(rotation, position);
	physx::PxRigidStatic* body = mPhysics->createRigidStatic(transform);
	mScene->addActor(*body);
	return body;
}

physx::PxRigidDynamic* PhysicsSystem::createDynamicBody(ci::vec3& position, ci::quat& rotation) {
	physx::PxTransform transform = sitara::ecs::physics::to(rotation, position);
	physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(transform);
	mScene->addActor(*body);
	return body;
}

int PhysicsSystem::registerMaterial(float staticFriction, float dynamicFriction, float restitution) {
	mMaterialCount++;
	physx::PxMaterial* material = mPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
	mMaterialRegistry.insert(std::pair<int, physx::PxMaterial*>(mMaterialCount, material));
	return mMaterialCount;
}

physx::PxMaterial* PhysicsSystem::getMaterial(int id) {
	return mMaterialRegistry[id];
}