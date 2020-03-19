#pragma once

#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct Separation {
			float mWeight;
			float mZoneRadius;
			float mZoneAngle;
		};
	}
}
