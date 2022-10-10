#pragma once

#include "entityx/Entity.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"

namespace sitara {
    namespace ecs {
        struct Tween {
            struct Options {
                Options(float duration = 0.5, float delay = 0) {
                    mTweenDuration = duration;
                    mTweenDelay = delay;
                    mEasingFn = ci::EaseInOutCubic();
                    mStartFn = nullptr;
                    mUpdateFn = nullptr;
                    mFinishFn = nullptr;
                }

                float mTweenDuration;
                float mTweenDelay;
                ci::EaseFn mEasingFn;
                std::function<void(entityx::Entity&)> mStartFn;
                std::function<void(entityx::Entity&)> mUpdateFn;
                std::function<void(entityx::Entity&)> mFinishFn;
            };

        Tween() {
                mPosition = ci::vec3(0);
                mScale = ci::vec3(0);
                mRotationAxis = ci::vec3(0);
                mRotationAngle = 0;

                mTintColor = ci::ColorA::white();

                mTimeline = ci::Timeline::create();
            }

            // transform component
            ci::Anim<ci::vec3> mPosition;
            ci::Anim<ci::vec3> mScale;
            ci::Anim<ci::vec3> mRotationAxis;
            ci::Anim<float> mRotationAngle;

            // tint color
            ci::Anim<ci::ColorA> mTintColor;

            // text component

            // fbo component
            //ci::Anim<ci::vec3> mFboCameraRotationAxis;
            //ci::Anim<float> mFboCameraRotationAngle;
            //ci::Anim<float> mFboCameraDistance;

            ci::TimelineRef mTimeline;

            friend class TimelineSystem;
        };

        typedef entityx::ComponentHandle<Tween> TweenHandle;
    }  // namespace ecs
}