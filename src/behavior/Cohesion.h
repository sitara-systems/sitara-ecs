#pragma once

#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct Cohesion {
			float mWeight;
			float mZoneRadius;
			float mZoneAngle;
		};
	}
}