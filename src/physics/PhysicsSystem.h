#pragma once

#include "entityx/System.h"
#include "cinder/Vector.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "physics/RigidBody.h"

namespace sitara {
  namespace ecs {
    class PhysicsSystem : public entityx::System<PhysicsSystem>, public entityx::Receiver<PhysicsSystem> {
    public:
	  void configure(entityx::EntityManager &entities, entityx::EventManager &events) override;
      void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
	  void receive(const entityx::ComponentAddedEvent<sitara::ecs::RigidBody>& event);
	  void receive(const entityx::ComponentRemovedEvent<sitara::ecs::RigidBody>& event);
	  void setGravity(ci::vec3 gravity);
	  btDiscreteDynamicsWorld* getWorld();
    private:
      btDefaultCollisionConfiguration* mCollisionConfiguration;
      btCollisionDispatcher* mDispatcher;
      btBroadphaseInterface* mOverlappingPairCache;
      btSequentialImpulseConstraintSolver* mBulletSolver;
      btDiscreteDynamicsWorld* mDynamicsWorld;
    };
  }
}
