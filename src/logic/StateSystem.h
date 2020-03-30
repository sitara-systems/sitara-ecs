#pragma once

#include "entityx/System.h"
#include "LogicState.h"

namespace sitara {
  namespace ecs {
    class StateSystem : public entityx::System<StateSystem> {
    public:
		void configure(entityx::EntityManager& entities, entityx::EventManager& events);
		void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
		static void setState(entityx::ComponentHandle<LogicState> handle, int stateId);
    private:
    };
  }
}
