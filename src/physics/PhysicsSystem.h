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
	  void setGravity(ci::vec3 gravity);
	  std::shared_ptr<btDiscreteDynamicsWorld> getWorld();
    private:
      std::shared_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
      std::shared_ptr<btCollisionDispatcher> mDispatcher;
      std::shared_ptr<btBroadphaseInterface> mOverlappingPairCache;
      std::shared_ptr<btSequentialImpulseConstraintSolver> mBulletSolver;
      std::shared_ptr<btDiscreteDynamicsWorld> mDynamicsWorld;
    };
  }
}
