#pragma once

#include "entityx/Entity.h"

namespace sitara {
	namespace ecs {
		struct LogicalLayer {
		public:
			LogicalLayer() = delete;

			LogicalLayer(int layerId) : mLayerId(layerId) {
			}

			int mLayerId;
		};
	}
}
