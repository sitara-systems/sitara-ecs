#pragma once

#include "cinder/app/App.h"
#include "entityx/System.h"

namespace sitara {
	namespace ecs {
        class MouseSystem : public entityx::System<MouseSystem>
        {
        public:
          explicit MouseSystem(entityx::EntityManager &entities)
          : mEntities(entities)
          {}

          void configure(entityx::EventManager &events) override;
          void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {}

          void mouseDown(ci::app::MouseEvent& event);
          void mouseDrag(ci::app::MouseEvent& event);
          void mouseMove(ci::app::MouseEvent& event);
          void mouseUp(ci::app::MouseEvent& event);
          void mouseWheel(ci::app::MouseEvent& event);

          void onDown(const ci::vec3& mousePosition, entityx::Entity selectedEntity);

        private:
          using ScopedConnectionRef = std::shared_ptr<ci::signals::ScopedConnection>;
          std::vector<ScopedConnectionRef> mSignals;
          entityx::EntityManager &mEntities;
          entityx::Entity mSelectedEntity;
          ci::vec3 mEntityStartPosition;
          ci::vec3 mDragStartPosition;
        };
    }
}
