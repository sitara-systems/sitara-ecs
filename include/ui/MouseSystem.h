#pragma once

#include "cinder/app/App.h"
#include "geometry/Geometry.h"
#include "transform/Transform.h"
#include "transform/TransformSystem.h"
#include "ui/Clickable2D.h"
#include <functional>

#include "entityx/System.h"

namespace sitara {
	namespace ecs {
        struct DragData {
            ci::vec3 mEntityStartPosition;
            ci::vec3 mDragStartPosition;
            ci::vec2 mCurrentMousePosition;
        };

        class MouseSystem : public entityx::System<MouseSystem>
        {
        public:
            MouseSystem() = delete;

          explicit MouseSystem(entityx::EntityManager& entities, entityx::SystemManager& systems)
          : mEntities(entities), mSystems(systems)
          {}

          void configure(entityx::EventManager &events) override;
          void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {}

          virtual void mouseDown(ci::app::MouseEvent& event);
          virtual void mouseDrag(ci::app::MouseEvent& event);
          virtual void mouseMove(ci::app::MouseEvent& event);
          virtual void mouseUp(ci::app::MouseEvent& event);
          virtual void mouseWheel(ci::app::MouseEvent& event);

        private:
          using ScopedConnectionRef = std::shared_ptr<ci::signals::ScopedConnection>;
          std::vector<ScopedConnectionRef> mSignals;
          entityx::EntityManager& mEntities;
          entityx::SystemManager& mSystems;
          entityx::Entity mSelectedEntity;
          DragData mDragData;
        };
    }
}
