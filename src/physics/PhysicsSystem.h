#pragma once

#include "entityx/System.h"

namespace sitara {
  namespace ecs {
    class PhysicsSystem : public entityx::System<PhysicsSystem> {
    public:
      void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
    private:
      entityx::TimeDelta mPreviousDelta = 1.0 / 60.0;
	  float mMaxSpeed = 1.0f;
    };

  }
}
