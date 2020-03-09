#pragma once

#include "entityx/System.h"

namespace sitara {
  namespace ecs {
    class TransformSystem : public entityx::System<TransformSystem> {
    public:
      void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
    private:
    };
  }
}