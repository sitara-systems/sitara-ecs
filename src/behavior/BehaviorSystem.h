#pragma once

#include "entityx/System.h"
#include "cinder/Vector.h"

namespace sitara {
	namespace ecs {
		class BehaviorSystem : public entityx::System<BehaviorSystem> {
		public:
			void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
			void seek(entityx::Entity& entity);
			void flee(entityx::Entity& entity, ci::vec3 nullDirection = ci::vec3(0, 0, 1));
			void arrive(entityx::Entity& entity);
			void wander(entityx::Entity& entity);
			void separate(entityx::Entity& entity, entityx::EntityManager& entities);
			void cohere(entityx::Entity& entity, entityx::EntityManager& entities);
			void align(entityx::Entity& entity, entityx::EntityManager& entities);
		private:
		};
	}
}