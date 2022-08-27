#pragma once

#include "entityx/System.h"
#include "Transform.h"

namespace sitara {
  namespace ecs {
    class TransformSystem : public entityx::System<TransformSystem>, public entityx::Receiver<TransformSystem> {
    public:
        TransformSystem();
		void configure(entityx::EntityManager& entities, entityx::EventManager& events);
		void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
		entityx::ComponentHandle<Transform> attachChild(entityx::Entity parent, entityx::Entity child);
		void attachChild(entityx::ComponentHandle<Transform> parentHandle, entityx::ComponentHandle<Transform> childHandle);
		void removeFromParent(entityx::ComponentHandle<Transform> childHandle);
        void applyToRootNodes(entityx::EntityManager& entities,
                                      const std::function<void(const entityx::ComponentHandle<Transform>,
                                                               entityx::ComponentHandle<Transform>)>& function);
        entityx::ComponentHandle<sitara::ecs::Transform> getNodeByLabel(entityx::EntityManager& entities, std::string label);
        void ascend(entityx::ComponentHandle<Transform> childHandle, const std::function<void(const entityx::ComponentHandle<Transform>, entityx::ComponentHandle<Transform>)>& function);
        void descend(entityx::ComponentHandle<Transform> rootHandle, const std::function<void(const entityx::ComponentHandle<Transform>, entityx::ComponentHandle<Transform>)>& function);
		void receive(const entityx::ComponentRemovedEvent<Transform>& event);
        void enableDepthSort(bool enabled);
    private:
        bool mDepthSortEnabled;
    };
  }
}