#pragma once

#include <functional>
#include "Particle.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"

namespace sitara {
	namespace ecs {
		class Force {
		public:
			Force(std::function<ci::vec3(const Particle&)> fn) {

			}
		protected:
			std::function<ci::vec3(const Particle&)> mForceFunction;
			ci::vec3 mSourcePosition;
			bool mIsOn;
			float mMinForce;
			float mMaxForce;
		}
	}
}