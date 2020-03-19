#pragma once

#include "entityx/System.h"

namespace sitara {
	namespace ecs {
		class BehaviorSystem : public entityx::System<BehaviorSystem> {
		public:
			void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
		private:
		};
	}
}