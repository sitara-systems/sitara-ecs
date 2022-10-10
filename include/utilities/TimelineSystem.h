#pragma once

#include <queue>
#include "Tween.h"
#include "cinder/Timeline.h"
#include "entityx/System.h"

namespace sitara {
namespace ecs {
class TimelineSystem : public entityx::System<TimelineSystem>, public entityx::Receiver<TimelineSystem> {
    struct TimelineCallback {
        uint64_t mEntityId;
        std::function<void(entityx::Entity&)> mFn;
    };

   public:
    TimelineSystem();
    void configure(entityx::EntityManager& entities, entityx::EventManager& events);
    void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
    void tweenPosition(entityx::Entity& entity, ci::vec3& targetPosition, const sitara::ecs::Tween::Options& options);
    void tweenTint(entityx::Entity& entity, ci::ColorA targetTint, const sitara::ecs::Tween::Options& options);
    void tweenAlpha(entityx::Entity& entity, float targetAlpha, const sitara::ecs::Tween::Options& options);
   private:
    std::queue<TimelineCallback> mStartFns;
    std::queue<TimelineCallback> mUpdateFns;
    std::queue<TimelineCallback> mFinishFns;
};
}  // namespace ecs
}  // namespace sitara