#pragma once

#include "Fbo.h"
#include "entityx/System.h"

namespace sitara {
    namespace ecs {
        class FboSystem : public entityx::System<FboSystem>, public entityx::Receiver<FboSystem> {
           public:
            FboSystem();
            void configure(entityx::EntityManager& entities, entityx::EventManager& events);
            void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
           private:
        };
    }  // namespace ecs
}  // namespace sitara