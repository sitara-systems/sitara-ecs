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
        TransformHandle attachChild(entityx::Entity parent, entityx::Entity child);
        void attachChild(TransformHandle parentHandle, TransformHandle childHandle);
        void removeFromParent(TransformHandle childHandle);
        void applyToRootNodes(entityx::EntityManager& entities,
            const std::function<void(const TransformHandle, TransformHandle)>& function);
        TransformHandle getNodeByLabel(entityx::EntityManager& entities, std::string label);
        void ascend(TransformHandle childHandle,
                    const std::function<void(const TransformHandle,
                                             TransformHandle)>& function);
        void descend(TransformHandle rootHandle, const std::function<void(const TransformHandle, TransformHandle)>& function);
		void receive(const entityx::ComponentRemovedEvent<Transform>& event);
        void enableDepthSort(bool enabled);
        std::vector<std::pair<std::string, bool>> getLabelTree(entityx::EntityManager& entities);
    private:
        bool mDepthSortEnabled;
    };
  }
}