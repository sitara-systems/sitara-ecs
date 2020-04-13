#include "physics/PhysicsSystem.h"
#include "transform/Transform.h"

using namespace sitara::ecs;

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

void PhysicsSystem::configure(entityx::EntityManager &entities, entityx::EventManager &events) {
	  ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	  mCollisionConfiguration = new btDefaultCollisionConfiguration();
	  ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	  mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	  ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	  mOverlappingPairCache = new btDbvtBroadphase();
	  ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	  mBulletSolver = new btSequentialImpulseConstraintSolver();
	  mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mBulletSolver, mCollisionConfiguration);
	  mDynamicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));

	  mMaximumVelocity = 0;
	  mElapsedSimulationTime = 0.0f;

	  events.subscribe<entityx::ComponentAddedEvent<RigidBody>>(*this);
}

void PhysicsSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;

	for (auto entity : entities.entities_with_components(body, transform)) {
		// check for maximum velocity
		if (mMaximumVelocity) {
			ci::vec3 currentVelocity = physics::fromBtVector3(body->getRigidBody()->getLinearVelocity());
			if (glm::length(currentVelocity) > mMaximumVelocity) {
				//std::printf("Limiting Velocity\n");
				body->getRigidBody()->setLinearVelocity(physics::toBtVector3(mMaximumVelocity * glm::normalize(currentVelocity)));
			}

			ci::vec3 currentForce = physics::fromBtVector3(body->getRigidBody()->getTotalForce());
			if (glm::length(currentForce) > mMaximumVelocity) {
				body->getRigidBody()->clearForces();
				body->getRigidBody()->applyCentralForce(physics::toBtVector3(mMaximumVelocity * glm::normalize(currentForce)));
			}

		}

		// update transform component with new world transform
		btTransform trans;
		if (body->getMotionState()) {
			body->getMotionState()->getWorldTransform(trans);
		}

		transform->mPosition = physics::fromBtVector3(trans.getOrigin());
		transform->mOrientation = physics::fromBtQuaternion(trans.getRotation());
	}

	float timeStep = static_cast<float>(dt);
	mDynamicsWorld->stepSimulation(timeStep, 10);
	mElapsedSimulationTime += timeStep;
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::RigidBody>& event) {
	mDynamicsWorld->addRigidBody(event.component->getRigidBody());
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::RigidBody>& event) {
	mDynamicsWorld->removeCollisionObject(event.component->getRigidBody());
	mDynamicsWorld->removeRigidBody(event.component->getRigidBody());

}

double PhysicsSystem::getElapsedSimulationTime() {
	return mElapsedSimulationTime;
}


void PhysicsSystem::setGravity(ci::vec3 gravity) {
	if (mDynamicsWorld != nullptr) {
		mDynamicsWorld->setGravity(physics::toBtVector3(gravity));
	}
	else {
		std::printf("sitara::ecs::PhysicsSystem ERROR -- must configure() system before you can set parameters.\n");
	}
}

void PhysicsSystem::setMaximumVelocity(float velocity) {
	mMaximumVelocity = velocity;
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