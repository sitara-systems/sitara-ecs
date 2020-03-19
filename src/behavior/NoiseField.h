#pragma once

#include "utilities/Simplex.h"
#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		struct NoiseField {
			NoiseField(ci::vec4 offset = ci::vec4(0), float weight = 10.0f) :
				mWeight(weight),
				mOffsets(offset)
			{

			}

			ci::vec4 mOffsets;
			float mWeight;
		};
	}
}
