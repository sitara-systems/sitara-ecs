#pragma once
#pragma once

#include "cinder/gl/gl.h"
#include "physics/RigidBody.h"

namespace sitara {
	namespace ecs {
		struct MovingTarget {
			MovingTarget(entityx::ComponentHandle<RigidBody> bodyHandle) : mRigidBody(bodyHandle) {

			}

			entityx::ComponentHandle<RigidBody> mRigidBody;
		};
	}
}
