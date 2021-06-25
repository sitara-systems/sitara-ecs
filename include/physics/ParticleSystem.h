#pragma once

#include "entityx/System.h"
#include "Particle.h"
#include "Attractor.h"
#include "Spring.h"

namespace sitara {
	namespace ecs {
        class ParticleSystem : public entityx::System<ParticleSystem>, public entityx::Receiver<ParticleSystem> {
        public:
            ParticleSystem();
            ~ParticleSystem();
            void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
            void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
            //void receive(const entityx::ComponentAddedEvent<Particle>& event);
            //void receive(const entityx::ComponentRemovedEvent<Particle>& event);
            //void receive(const entityx::ComponentAddedEvent<Spring>& event);
            //void receive(const entityx::ComponentRemovedEvent<Spring>& event);
            double getElapsedSimulationTime();
        };
    }
}
