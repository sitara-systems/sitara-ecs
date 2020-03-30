#pragma once
#pragma once

#include "cinder/gl/gl.h"
#include "transform/Transform.h"

namespace sitara {
	namespace ecs {
		struct MovingTarget {
			MovingTarget(entityx::ComponentHandle<Transform> targetTransform) : mTargetTransform(targetTransform) {

			}

			ci::vec3 mPreviousPosition;
			entityx::ComponentHandle<Transform> mTargetTransform;
		};
	}
}
