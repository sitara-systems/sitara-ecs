#include "StateSystem.h"

using namespace sitara::ecs;

void StateSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {

}

void StateSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::LogicState> state;

	for (entityx::Entity e : entities.entities_with_components(state)) {
		state->update();
	}
}
