#include "transform/TransformSystem.h"
#include "transform/Transform.h"

using namespace cinder;
using namespace sitara::ecs;

void TransformSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

	for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
		if (transformHandle->isRoot()) {
			transformHandle->updateWorldTransform(mat4(1));
			transformHandle->descend([] (const sitara::ecs::Transform &parent, sitara::ecs::Transform &child) {
				child.updateWorldTransform(parent.getWorldTransform());
			});
		}
	}
}