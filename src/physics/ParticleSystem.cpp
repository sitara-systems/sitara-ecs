#include "physics/ParticleSystem.h"
#include "transform/Transform.h"

using namespace sitara::ecs;

ParticleSystem::ParticleSystem() {

}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
}

void ParticleSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::Attractor> attractor;
	entityx::ComponentHandle<sitara::ecs::Particle> particle;
	entityx::ComponentHandle<sitara::ecs::Spring> spring;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;

	for (auto entity : entities.entities_with_components(particle)) {
		particle->clearForces();

		ci::vec3 v = particle->getVelocity();
		// add drag
		v *= -0.5f;
		particle->addForce(v);
	}

	for (auto entity : entities.entities_with_components(attractor)) {
		if (attractor->IsOn()) {
			for (auto entity : entities.entities_with_components(particle)) {
				attractor->apply(particle);
			}
		}
	}

	for (auto entity : entities.entities_with_components(spring)) {
		spring->apply();
	}

	float tt = 0.5f*dt*dt;

	for (auto entity : entities.entities_with_components(particle)) {
		ci::vec3 force = particle->getForces();
		float magnitude = glm::length(force);
		magnitude = physics::clampParticleForce(magnitude, 1000.0f);
		ci::vec3 direction = glm::normalize(force);

		ci::vec3 correctedForce = magnitude * direction;
		ci::vec3 velocity = particle->getVelocity();
		ci::vec3 position = particle->getPosition();
		ci::vec3 acceleration = correctedForce * 1.0f / particle->getMass();

		position += velocity * float(dt);
		position += acceleration * tt;

		velocity += acceleration * float(dt);

		particle->setVelocity(velocity);
		particle->setPosition(position);
	}

	for (auto entity : entities.entities_with_components(particle, transform)) {
			transform->mPosition = particle->getPosition();
	}

	for (auto entity : entities.entities_with_components(attractor, transform)) {
		transform->mPosition = attractor->getPosition();
	}
}

double ParticleSystem::getElapsedSimulationTime() {
	return 0.0;
}