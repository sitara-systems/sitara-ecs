#include "utilities/TimelineSystem.h"
#include "transform/Transform.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

using namespace sitara::ecs;

TimelineSystem::TimelineSystem() {

}

void TimelineSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {}

void TimelineSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
    entityx::ComponentHandle<sitara::ecs::Tween> tweenHandle;
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

    for (entityx::Entity e : entities.entities_with_components(tweenHandle, transformHandle)) {
        // update the timeline
        if (!(tweenHandle->mTimeline->isComplete())) {
            tweenHandle->mTimeline->stepTo(static_cast<float>(ci::app::getElapsedSeconds()));
        }

        // Transform Component animations
        if (!tweenHandle->mPosition.isComplete()) {
            transformHandle->mPosition = tweenHandle->mPosition.value();
        }

        if (!tweenHandle->mScale.isComplete()) {
            transformHandle->mScale = tweenHandle->mScale.value();
        }

        if (!(tweenHandle->mRotationAngle.isComplete() || tweenHandle->mRotationAxis.isComplete())) {
            transformHandle->mOrientation =
                glm::angleAxis(tweenHandle->mRotationAngle.value(), tweenHandle->mRotationAxis.value());
        }

        if (!tweenHandle->mTintColor.isComplete()) {
            transformHandle->mTintColor = tweenHandle->mTintColor.value();
        }
    }

    while (!mStartFns.empty()) {
        TimelineCallback cb = mStartFns.front();
        entityx::Entity e = entities.get(entityx::Entity::Id::Id(cb.mEntityId));
        cb.mFn(e);
        mStartFns.pop();
    }

    while (!mUpdateFns.empty()) {
        TimelineCallback cb = mUpdateFns.front();
        entityx::Entity e = entities.get(entityx::Entity::Id::Id(cb.mEntityId));
        cb.mFn(e);
        mUpdateFns.pop();
    }

    while (!mFinishFns.empty()) {
        TimelineCallback cb = mFinishFns.front();
        entityx::Entity e = entities.get(entityx::Entity::Id::Id(cb.mEntityId));
        cb.mFn(e);
        mFinishFns.pop();
    }
}

void TimelineSystem::tweenPosition(entityx::Entity& entity, ci::vec3& targetPosition, const sitara::ecs::Tween::Options& options) {
    entityx::ComponentHandle<sitara::ecs::Tween> tweenHandle;
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

    entity.unpack(tweenHandle, transformHandle);

    if (tweenHandle && transformHandle) {
        uint64_t id = entity.id().id();
        tweenHandle->mPosition = transformHandle->mPosition;  // set initial value
        auto tween = tweenHandle->mTimeline->apply(&tweenHandle->mPosition, targetPosition, options.mTweenDuration, options.mEasingFn);
        if (options.mTweenDelay) {
            tween.delay(options.mTweenDelay);
        }
        if (options.mStartFn) {
            tween.startFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mStartFn;
                mStartFns.push(cb);
            });
        }
        if (options.mUpdateFn) {
            /*
             * TO DO: probably a smarter way to do this than push back to the queue every time
             */
            tween.updateFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mUpdateFn;
                mUpdateFns.push(cb);
            });
        }
        if (options.mFinishFn) {
            tween.finishFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mFinishFn;
                mFinishFns.push(cb);
            });
        }
    } else {
        CI_LOG_W("No tween or transform component found on entity; no timeline event will be scheduled.");
    }
}

void sitara::ecs::TimelineSystem::tweenTint(entityx::Entity& entity,
                                            ci::ColorA targetTint,
                                            const sitara::ecs::Tween::Options& options) {
    entityx::ComponentHandle<sitara::ecs::Tween> tweenHandle;
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

    entity.unpack(tweenHandle, transformHandle);

    if (tweenHandle && transformHandle) {
        uint64_t id = entity.id().id();
        tweenHandle->mTintColor = transformHandle->mTintColor;  // set initial value
        auto tween = tweenHandle->mTimeline->apply(&tweenHandle->mTintColor, targetTint, options.mTweenDuration,
                                                   options.mEasingFn);
        if (options.mTweenDelay) {
            tween.delay(options.mTweenDelay);
        }
        if (options.mStartFn) {
            tween.startFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mStartFn;
                mStartFns.push(cb);
            });
        }
        if (options.mUpdateFn) {
            /*
             * TO DO: probably a smarter way to do this than push back to the queue every time
             */
            tween.updateFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mUpdateFn;
                mUpdateFns.push(cb);
            });
        }
        if (options.mFinishFn) {
            tween.finishFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mFinishFn;
                mFinishFns.push(cb);
            });
        }
    } else {
        CI_LOG_W("No tween or transform component found on entity; no timeline event will be scheduled.");
    }
}

void sitara::ecs::TimelineSystem::tweenAlpha(entityx::Entity& entity,
                                             float targetAlpha,
                                             const sitara::ecs::Tween::Options& options) {
    entityx::ComponentHandle<sitara::ecs::Tween> tweenHandle;
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

    entity.unpack(tweenHandle, transformHandle);

    if (tweenHandle && transformHandle) {
        uint64_t id = entity.id().id();
        tweenHandle->mTintColor = transformHandle->mTintColor;  // set initial value

        ci::ColorA targetTint = tweenHandle->mTintColor;
        targetTint.a = targetAlpha;

        auto tween = tweenHandle->mTimeline->apply(&tweenHandle->mTintColor, targetTint, options.mTweenDuration,
                                                   options.mEasingFn);

        if (options.mTweenDelay) {
            tween.delay(options.mTweenDelay);
        }
        if (options.mStartFn) {
            tween.startFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mStartFn;
                mStartFns.push(cb);
            });
        }
        if (options.mUpdateFn) {
            /*
             * TO DO: probably a smarter way to do this than push back to the queue every time
             */
            tween.updateFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mUpdateFn;
                mUpdateFns.push(cb);
            });
        }
        if (options.mFinishFn) {
            tween.finishFn([&, id, options]() {
                TimelineCallback cb;
                cb.mEntityId = id;
                cb.mFn = options.mFinishFn;
                mFinishFns.push(cb);
            });
        }
    } else {
        CI_LOG_W("No tween or transform component found on entity; no timeline event will be scheduled.");
    }
}