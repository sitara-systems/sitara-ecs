#pragma once

#include "cinder/gl/gl.h"
#include "transform/Transform.h"

namespace sitara {
	namespace ecs {
		struct Target {
			Target(ci::vec3 position = ci::vec3(0), float weight = 10.0f, float slowingDistance = 0.0f) :
				mTargetPosition(position),
				mWeight(weight),
				mSlowingDistance(slowingDistance),
				mPreviousPosition(ci::vec3(0, 0, 0)),
				mTargetTransform()
			{
			}

			Target(entityx::ComponentHandle<sitara::ecs::Transform> transform, float weight = 10.0f, float slowingDistance = 0.0f) :
				mTargetTransform(transform),
				mWeight(weight),
				mSlowingDistance(slowingDistance),
				mTargetPosition(ci::vec3(0, 0, 0)),
				mPreviousPosition(ci::vec3(0, 0, 0))
			{
			}

			void update() {
				if (mTargetTransform.valid()) {
					ci::vec3 velocity = mTargetTransform->mPosition - mPreviousPosition;
					mTargetPosition = mTargetTransform->mPosition + velocity;
					mPreviousPosition = mTargetTransform->mPosition;
				}
			}

			void setTarget(entityx::ComponentHandle<sitara::ecs::Transform> targetTransform) {
				mTargetTransform = targetTransform;
			}

			void setTargetPosition(ci::vec3 target) {
				mTargetPosition = target;
				mTargetTransform = entityx::ComponentHandle<sitara::ecs::Transform>();
			}

			ci::vec3 getTargetPosition() {
				return mTargetPosition;
			}

			float mWeight;
			float mSlowingDistance;
		private:
			entityx::ComponentHandle<sitara::ecs::Transform> mTargetTransform;
			ci::vec3 mTargetPosition;
			ci::vec3 mPreviousPosition;
		};
	}
}
