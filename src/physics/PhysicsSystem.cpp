#include "physics/PhysicsSystem.h"
#include "transform/Transform.h"

using namespace sitara::ecs;

PhysicsSystem::PhysicsSystem() {
	mCollisionConfiguration = nullptr;
	mDispatcher = nullptr;
	mOverlappingPairCache = nullptr;
	mBulletSolver = nullptr;
	mDynamicsWorld = nullptr;
	mElapsedSimulationTime = 0.0;
	mEnableGhostCollisions = false;
}


PhysicsSystem::~PhysicsSystem() {
	if (mCollisionConfiguration != 0) {
		delete mCollisionConfiguration;
	}
	if (mDispatcher != 0) {
		delete mDispatcher;
	}
	if (mOverlappingPairCache != 0) {
		delete mOverlappingPairCache;
	}
	if (mBulletSolver != 0) {
		delete mBulletSolver;
	}
	if (mDynamicsWorld != 0) {
		delete mDynamicsWorld;
	}
}

void PhysicsSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();
	if (mEnableGhostCollisions) {
		mOverlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	}
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mBulletSolver = new btSequentialImpulseConstraintSolver();
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mBulletSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));

	mElapsedSimulationTime = 0.0f;

	events.subscribe<entityx::ComponentAddedEvent<RigidBody>>(*this);
	events.subscribe<entityx::ComponentRemovedEvent<RigidBody>>(*this);
	events.subscribe<entityx::ComponentAddedEvent<GhostBody>>(*this);
	events.subscribe<entityx::ComponentRemovedEvent<GhostBody>>(*this);
}

void PhysicsSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;

	// preprocessing callbacks
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mRigidBodyPreUpdateFns) {
			callback(body);
		}
	}

	// run simulation
	float timeStep = static_cast<float>(dt);
	mDynamicsWorld->stepSimulation(timeStep, 10);
	mElapsedSimulationTime += timeStep;

	// update transforms
	if (mEnableGhostCollisions) {
		// update Ghost Object transforms into ecs world
		checkGhostBodyCollisions();

		entityx::ComponentHandle<sitara::ecs::GhostBody> ghost;
		for (auto entity : entities.entities_with_components(ghost, transform)) {
			ghost->applyCollisionFunctions(ghost);
			// opposite of rigid body -- ghost body takes its transform FROM the transform component
			btTransform btTrans = physics::toBtTransform(transform->getWorldTransform());

			// flip rotations -- bullet and cinder use different angles
			float yaw, pitch, roll;
			btTrans.getRotation().getEulerZYX(yaw, pitch, roll);
			btQuaternion quat;
			quat.setEulerZYX(roll, pitch, yaw);
			btTrans.setRotation(quat);

			ghost->getGhostBody()->setWorldTransform(btTrans);
		}
	}

	// update transform component with new world transform from bullet
	for (auto entity : entities.entities_with_components(body, transform)) {
		btTransform trans;
		if (body->getRigidBody()) {
			trans = body->getRigidBody()->getWorldTransform();
		}

		transform->mPosition = physics::fromBtVector3(trans.getOrigin());
		transform->mOrientation = physics::fromBtQuaternion(trans.getRotation());

		// flip rotations -- bullet and cinder use different angles
		float yaw = transform->mOrientation.z;
		transform->mOrientation.z = transform->mOrientation.x;
		transform->mOrientation.x = yaw;
	}

	// post processing
	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mRigidBodyPostUpdateFns) {
			callback(body);
		}
	}
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::RigidBody>& event) {
	mDynamicsWorld->addRigidBody(event.component->getRigidBody());
	mRigidBodyMap.insert(std::make_pair(event.component->getRigidBody(), event.component));
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::RigidBody>& event) {
	mRigidBodyMap.erase(event.component->getRigidBody());
	mDynamicsWorld->removeCollisionObject(event.component->getRigidBody());
	mDynamicsWorld->removeRigidBody(event.component->getRigidBody());
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::GhostBody>& event) {
	mDynamicsWorld->addCollisionObject(event.component->getGhostBody(), btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);
	mGhostBodyMap.insert(std::make_pair(event.component->getGhostBody(), event.component));
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::GhostBody>& event) {
	mGhostBodyMap.erase(event.component->getGhostBody());
	mDynamicsWorld->removeCollisionObject(event.component->getGhostBody());
}

double PhysicsSystem::getElapsedSimulationTime() {
	return mElapsedSimulationTime;
}

bool PhysicsSystem::setGhostCollisions(bool enable) {
	mEnableGhostCollisions = enable;
	return mEnableGhostCollisions;
}

void PhysicsSystem::setGravity(ci::vec3 gravity) {
	if (mDynamicsWorld != nullptr) {
		mDynamicsWorld->setGravity(physics::toBtVector3(gravity));
	}
	else {
		std::printf("sitara::ecs::PhysicsSystem ERROR -- must configure() system before you can set parameters.\n");
	}
}

void PhysicsSystem::clearForces(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;

	for (auto entity : entities.entities_with_components(body)) {
		body->getRigidBody()->clearForces();
		body->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		body->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	}
}

btDiscreteDynamicsWorld* PhysicsSystem::getWorld() {
	return mDynamicsWorld;
}

void PhysicsSystem::checkGhostBodyCollisions() {
	mActiveGhostObjects.clear();

	entityx::ComponentHandle<GhostBody> ghostBodyComponent;

	// one pass to collate all collision objects
	btCollisionObjectArray collisionObjects = mDynamicsWorld->getCollisionObjectArray();
	for (int i = 0; i < collisionObjects.size(); i++) {
		btCollisionObject* obj = collisionObjects[i];
		// filter for ghost objects by upcasting
		btGhostObject* ghostBody = btGhostObject::upcast(obj);
		if (ghostBody) {
			auto gb_it = mGhostBodyMap.find(ghostBody);

			if (gb_it != mGhostBodyMap.end()) {
				ghostBodyComponent = gb_it->second;
			}
			if (ghostBodyComponent.valid()) {
				if (ghostBody->getNumOverlappingObjects() > 0) {
					// collision active
					mActiveGhostObjects.push_back(ghostBody);
					btCollisionObjectArray collisionRigidBodies = ghostBody->getOverlappingPairs();
					ghostBodyComponent->setCollisionState(true);
					ghostBodyComponent->resetCollisionBodies();
					for (int j = 0; j < collisionRigidBodies.size(); j++) {
						btCollisionObject* collisionObject = collisionRigidBodies[j];
						btRigidBody* collisionBody = btRigidBody::upcast(collisionObject);
						if (collisionBody) {
							entityx::ComponentHandle<RigidBody> rigidBodyComponent;
							auto rb_it = mRigidBodyMap.find(collisionBody);

							if (rb_it != mRigidBodyMap.end()) {
								rigidBodyComponent = rb_it->second;
								if (rigidBodyComponent.valid()) {
									ghostBodyComponent->addCollisionBodies(rigidBodyComponent);
								}
							}
						}
					}
				}
				else {
					// no collision
					if (ghostBodyComponent.valid()) {
						ghostBodyComponent->setCollisionState(false);
						ghostBodyComponent->resetCollisionBodies();
					}
				}
			}
		}
	}
}

void PhysicsSystem::resetBody(entityx::ComponentHandle<sitara::ecs::RigidBody> body, ci::vec3 position) {
	if (body->getMotionState()) {
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(physics::toBtVector3(position));
		transform.setRotation(btQuaternion());

		body->getRigidBody()->setWorldTransform(transform);
		body->getRigidBody()->getMotionState()->setWorldTransform(transform);

		body->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		body->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		body->getRigidBody()->clearForces();
		body->getRigidBody()->activate(true);
	}
}

void PhysicsSystem::addRigidBodyPreUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
	mRigidBodyPreUpdateFns.push_back(callback);
}

void PhysicsSystem::addRigidBodyPostUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
	mRigidBodyPostUpdateFns.push_back(callback);
}
