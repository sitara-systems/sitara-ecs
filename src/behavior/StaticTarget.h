#pragma once

#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct StaticTarget {
			StaticTarget(ci::vec3 position = ci::vec3(0), float weight = 10.0f, float slowingDistance = 0.0f) : 
				mTargetPosition(position), 
				mWeight(weight),
				mSlowingDistance(slowingDistance)
			{
			}

			void setTarget(ci::vec3 target) {
				mTargetPosition = target;
			}

			ci::vec3 getTarget() {
				return mTargetPosition;
			}

			float mWeight;
			float mSlowingDistance;
		private:
			ci::vec3 mTargetPosition;

			friend class BehaviorSystem;
		};
	}
}
