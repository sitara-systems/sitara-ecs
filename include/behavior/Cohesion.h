#pragma once

#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct Cohesion {
			Cohesion(float zoneRadius = 100.0f, float weight = 10.0f) :
				mZoneRadius(zoneRadius),
				mWeight(weight)
			{

			}

			float mZoneRadius;
			float mWeight;
		};
	}
}
