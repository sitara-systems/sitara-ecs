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

	for (auto entity : entities.entities_with_components(body, transform)) {
		for (auto callback : mRigidBodyUpdateFns) {
			callback(body);
		}

		// update transform component with new world transform
		btTransform trans;
		if (body->getMotionState()) {
			body->getMotionState()->getWorldTransform(trans);
		}

		transform->mPosition = physics::fromBtVector3(trans.getOrigin());
		transform->mOrientation = physics::fromBtQuaternion(trans.getRotation());
	}

	if (mEnableGhostCollisions) {
		// update Ghost Object transforms into ecs world
		entityx::ComponentHandle<sitara::ecs::GhostBody> ghost;
		for (auto entity : entities.entities_with_components(ghost, transform)) {
			// update transform component with new world transform
			btTransform trans = ghost->getGhostBody()->getWorldTransform();

			transform->mPosition = physics::fromBtVector3(trans.getOrigin());
			transform->mOrientation = physics::fromBtQuaternion(trans.getRotation());
		}

		// check for Ghost Collisions and callbacks
		checkGhostBodyCollisions();
		for (int ghostBodyIndex = 0; ghostBodyIndex < mGhostCollisions.size(); ghostBodyIndex++) {
			btGhostObject* ghostBody = mActiveGhostObjects[ghostBodyIndex];
			auto collisions = mGhostCollisions[ghostBodyIndex];
			for (int i = 0; i < collisions.size(); i++) {
				btCollisionObject* collisionObject = collisions[i];
				btRigidBody* collisionBody = btRigidBody::upcast(collisionObject);
				if (collisionBody) {
					entityx::ComponentHandle<RigidBody> rigidBodyComponent;
					entityx::ComponentHandle<GhostBody> ghostBodyComponent;
					auto rb_it = mRigidBodyMap.find(collisionBody);
					auto gb_it = mGhostBodyMap.find(ghostBody);

					if (rb_it != mRigidBodyMap.end()) {
						rigidBodyComponent = rb_it->second;
					}
					if (gb_it != mGhostBodyMap.end()) {
						ghostBodyComponent = gb_it->second;
					}
					if (rigidBodyComponent.valid() && ghostBodyComponent.valid()) {
						ghostBodyComponent->applyCollisionFunctions(rigidBodyComponent);
					}
				}
			}
		}
	}

	float timeStep = static_cast<float>(dt);
	mDynamicsWorld->stepSimulation(timeStep, 10);
	mElapsedSimulationTime += timeStep;
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
	mGhostCollisions.clear();

	entityx::ComponentHandle<GhostBody> ghostBodyComponent;

	btCollisionObjectArray collisionObjects = mDynamicsWorld->getCollisionObjectArray();
	for (int i = 0; i < collisionObjects.size(); i++) {
		btCollisionObject* obj = collisionObjects[i];
		btGhostObject* ghostBody = btGhostObject::upcast(obj);
		if (ghostBody) {
			auto gb_it = mGhostBodyMap.find(ghostBody);

			if (gb_it != mGhostBodyMap.end()) {
				ghostBodyComponent = gb_it->second;
			}

			if (ghostBody->getNumOverlappingObjects() > 0) {
				// collision active
				mActiveGhostObjects.push_back(ghostBody);
				mGhostCollisions.push_back(ghostBody->getOverlappingPairs());
				if (ghostBodyComponent.valid()) {
					ghostBodyComponent->setCollisionState(true);
				}
			}
			else {
				// no collision
				if (ghostBodyComponent.valid()) {
					ghostBodyComponent->setCollisionState(false);
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

void PhysicsSystem::addRigidBodyUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
	mRigidBodyUpdateFns.push_back(callback);
}
