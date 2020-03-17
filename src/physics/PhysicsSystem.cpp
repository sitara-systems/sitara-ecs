#include "physics/PhysicsSystem.h"

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
	  mDynamicsWorld->setGravity(btVector3(0.0, 0.0, 0.0));

	  events.subscribe<entityx::ComponentAddedEvent<RigidBody>>(*this);
}

void PhysicsSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
	mDynamicsWorld->stepSimulation(static_cast<float>(dt), 10);

	entityx::ComponentHandle<sitara::ecs::RigidBody> body;

	for (auto entity : entities.entities_with_components(body)) {
		ci::vec3 p = body->getPosition();
	}
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<sitara::ecs::RigidBody>& event) {
	mDynamicsWorld->addRigidBody(event.component->getRigidBody());
}

void PhysicsSystem::receive(const entityx::ComponentRemovedEvent<sitara::ecs::RigidBody>& event) {
	mDynamicsWorld->removeCollisionObject(event.component->getRigidBody());
	mDynamicsWorld->removeRigidBody(event.component->getRigidBody());

}

void PhysicsSystem::setGravity(ci::vec3 gravity) {
	if (mDynamicsWorld != nullptr) {
		mDynamicsWorld->setGravity(physics::toBtVector3(gravity));
	}
	else {
		std::printf("sitara::ecs::PhysicsSystem ERROR -- must configure() system before you can set parameters.\n");
	}
}

btDiscreteDynamicsWorld* PhysicsSystem::getWorld() {
	return mDynamicsWorld;
}