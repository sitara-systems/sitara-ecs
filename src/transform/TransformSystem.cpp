#include "transform/TransformSystem.h"

using namespace cinder;
using namespace sitara::ecs;

void TransformSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
	events.subscribe<entityx::ComponentRemovedEvent<Transform>>(*this);
}

void TransformSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

	for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
		if (transformHandle->isRoot()) {
			transformHandle->updateWorldTransform(mat4(1));
			descend(transformHandle, [](const entityx::ComponentHandle<Transform> parent, entityx::ComponentHandle<Transform> child) {
				child->updateWorldTransform(parent->getWorldTransform());
			});
		}
	}
}

entityx::ComponentHandle<Transform> TransformSystem::attachChild(entityx::Entity parent, entityx::Entity child) {
	entityx::ComponentHandle<Transform> parentHandle = parent.component<Transform>();

	entityx::ComponentHandle<Transform> childHandle;

	if (child.has_component<Transform>()) {
		childHandle = child.component<Transform>();
	}
	else {
		childHandle = child.assign<Transform>();
	}

	attachChild(parentHandle, childHandle);
	return childHandle;
}

void TransformSystem::attachChild(entityx::ComponentHandle<Transform> parentHandle, entityx::ComponentHandle<Transform> childHandle) {
	if (childHandle.get() != parentHandle.get()) {
		removeFromParent(childHandle);
		childHandle->setParent(parentHandle);
		parentHandle->addChild(childHandle);
	}
}

void TransformSystem::removeFromParent(entityx::ComponentHandle<Transform> childHandle) {
	if (childHandle->getParent()) {
		childHandle->getParent()->removeChild(childHandle);
	}
}

void TransformSystem::descend(entityx::ComponentHandle<Transform> rootHandle, const std::function<void(const entityx::ComponentHandle<Transform>, entityx::ComponentHandle<Transform>)>& function) {
	for (auto& child : rootHandle->getChildren()) {
		function(rootHandle, child);
		descend(child, function);
	}
}

void TransformSystem::receive(const entityx::ComponentRemovedEvent<Transform>& event) {
	removeFromParent(event.component);

}