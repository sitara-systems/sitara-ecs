#pragma once

#include "entityx/System.h"

namespace sitara {
	namespace ecs {
		class BehaviorSystem : public entityx::System<BehaviorSystem> {
		public:
			void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
			void seek(entityx::EntityManager& entities);
			void flee(entityx::EntityManager& entities);
			void arrive(entityx::EntityManager& entities);
			void wander(entityx::EntityManager& entities);
			void separate(entityx::EntityManager& entities);
			void cohere(entityx::EntityManager& entities);
			void align(entityx::EntityManager& entities);
		private:
		};
	}
}