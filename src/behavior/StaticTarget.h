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

			float mWeight;
			ci::vec3 mTargetPosition;
			float mSlowingDistance;
		};
	}
}
