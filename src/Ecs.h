#pragma once

#include "entityx/config.h"
#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/quick.h"
#include "entityx/deps/Dependencies.h"

#include "transform/Transform.h"
#include "transform/TransformSystem.h"

#include "physics/RigidBody.h"
#include "physics/PhysicsSystem.h"
#include "physics/PhysicsUtils.h"

#include "geometry/Geometry.h"
#include "geometry/GeometryUtils.h"

#include "behavior/StaticTarget.h"
#include "behavior/MovingTarget.h"
#include "behavior/NoiseField.h"
#include "behavior/Separation.h"
#include "behavior/Cohesion.h"
#include "behavior/Alignment.h"
#include "behavior/BehaviorSystem.h"

#include "logic/LogicalLayer.h"

#include "utilities/Units.h"

namespace sitara {
	namespace ecs {
		void configureSystems(entityx::SystemManager& systems) {
			systems.add<entityx::deps::Dependency<RigidBody, Transform>>();
			systems.add<entityx::deps::Dependency<MovingTarget, StaticTarget>>();
		}
	}
}
