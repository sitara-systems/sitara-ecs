#pragma once

#include "utilities/Simplex.h"
#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct NoiseField {
			NoiseField(ci::vec4 multipliers = ci::vec4(ci::vec3(0.001f), 1.0f), ci::vec4 offset = ci::vec4(0), float weight = 10.0f) :
				mMultipliers(multipliers),
				mOffsets(offset),
				mWeight(weight)
			{

			}

			ci::vec4 mMultipliers;
			ci::vec4 mOffsets;
			float mWeight;
		};
	}
}
