#pragma once

#include "entityx/config.h"
#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/quick.h"
#include "entityx/deps/Dependencies.h"

#include "transform/Transform.h"
#include "transform/TransformSystem.h"

#include "physics/Particle.h"
#include "physics/ParticleSystem.h"

#include "physics/DynamicBody.h"
#include "physics/StaticBody.h"
#include "physics/OverlapDetector.h"
#include "physics/PhysicsSystem.h"
#include "physics/PhysicsUtils.h"

#include "geometry/Geometry.h"
#include "geometry/GeometryUtils.h"

#include "behavior/Target.h"
#include "behavior/NoiseField.h"
#include "behavior/Separation.h"
#include "behavior/Cohesion.h"
#include "behavior/Alignment.h"
#include "behavior/BehaviorSystem.h"

#include "logic/LogicalLayer.h"
#include "logic/LogicState.h"
#include "logic/StateSystem.h"

#ifdef USING_SDFTEXT
#include "text/Glyph.h"
#include "text/GlyphString.h"
#include "text/TextSystem.h"
#else

#endif

#include "ui/Clickable2D.h"
#include "ui/MouseSystem.h"

#include "utilities/Units.h"

namespace sitara {
	namespace ecs {
		inline void configureSystems(entityx::SystemManager& systems) {
			//systems.add<entityx::deps::Dependency<Target, Transform>>();
			systems.add<entityx::deps::Dependency<Particle, Transform>>();
			systems.add<entityx::deps::Dependency<Attractor, Transform>>();
			systems.add<entityx::deps::Dependency<DynamicBody, Transform>>();
			systems.add<entityx::deps::Dependency<StaticBody, Transform>>();
			systems.add<entityx::deps::Dependency<OverlapDetector, Transform>>();
            systems.add<entityx::deps::Dependency<Geometry, Transform>>();
            systems.add<entityx::deps::Dependency<Clickable2D, Transform>>();
        }
	}
}
