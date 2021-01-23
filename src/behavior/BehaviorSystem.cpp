#include "behavior/BehaviorSystem.h"
#include "behavior/Target.h"
#include "behavior/NoiseField.h"
#include "behavior/Separation.h"
#include "behavior/Cohesion.h"
#include "behavior/Alignment.h"
#include "physics/DynamicBody.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

using namespace sitara::ecs;

void BehaviorSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::Target> staticTarget;

	for (auto entity : entities.entities_with_components(staticTarget)) {
		staticTarget->update();
	}
}

void BehaviorSystem::seek(entityx::Entity& entity) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Target> target = entity.component<sitara::ecs::Target>();

	if (body.valid() && target.valid()) {
		ci::vec3 position = body->getPosition();
		ci::vec3 targetOffset = target->getTargetPosition() - position;
		ci::vec3 norm;
		if (glm::length(targetOffset) == 0) {
			norm = ci::vec3(0);
		}
		else {
			norm = glm::normalize(targetOffset);
		}
		ci::vec3 desiredVelocity = target->mWeight * norm;
		ci::vec3 currentVelocity = body->getVelocity();
		ci::vec3 desiredAcceleration = desiredVelocity - currentVelocity;
		body->applyForce(desiredAcceleration);
	}
}

void BehaviorSystem::flee(entityx::Entity& entity, ci::vec3 nullDirection) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Target> target = entity.component<sitara::ecs::Target>();

	if (body.valid() && target.valid()) {
		ci::vec3 position = body->getPosition();
		ci::vec3 targetOffset = position - target->getTargetPosition();
		ci::vec3 norm;
		if (glm::length(targetOffset) < 10) {
			norm = nullDirection;
		}
		else {
			norm = glm::normalize(targetOffset);
		}
		ci::vec3 desiredVelocity = target->mWeight * norm;
		ci::vec3 currentVelocity = body->getVelocity();
		ci::vec3 desiredAcceleration = desiredVelocity - currentVelocity;
		body->applyForce(desiredAcceleration);
	}
}

void BehaviorSystem::arrive(entityx::Entity& entity) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Target> target = entity.component<sitara::ecs::Target>();

	if (body.valid() && target.valid()) {
		ci::vec3 position = body->getPosition();			
		ci::vec3 t_p = target->getTargetPosition();
		ci::vec3 targetOffset = t_p - position;
		float distance = glm::length(targetOffset);
		ci::vec3 norm;
		if(glm::length(targetOffset) < 10) {
			norm = ci::vec3(0);
		}
		else {
			norm = glm::normalize(targetOffset);
		}
		
		ci::vec3 desiredVelocity;
		if (distance < target->mSlowingDistance) {
			// slow down
			desiredVelocity = target->mWeight * (distance / target->mSlowingDistance) * norm;
		}
		else {
			// regular seek behavior
			desiredVelocity = target->mWeight * norm;
		}
		ci::vec3 currentVelocity = body->getVelocity();
		ci::vec3 desiredAcceleration = desiredVelocity - currentVelocity;
		body->applyForce(desiredAcceleration);
	}
}

void BehaviorSystem::wander(entityx::Entity& entity) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::NoiseField> noise = entity.component<sitara::ecs::NoiseField>();

	if (body.valid() && noise.valid()) {
		ci::vec3 position = body->getPosition();
		ci::vec3 direction = ci::vec3(
			Simplex::noise(ci::vec2(position.x + noise->mOffsets.x, ci::app::getElapsedSeconds() + noise->mOffsets.w)),
			Simplex::noise(ci::vec2(position.y + noise->mOffsets.y, ci::app::getElapsedSeconds() + noise->mOffsets.w)),
			Simplex::noise(ci::vec2(position.z + noise->mOffsets.z, ci::app::getElapsedSeconds() + noise->mOffsets.w))
		);
		ci::vec3 norm;
		if (glm::length(direction) == 0) {
			norm = ci::vec3(0);
		}
		else {
			norm = glm::normalize(direction);
		}

		ci::vec3 desiredAcceleration = noise->mWeight * norm;
		body->applyForce(desiredAcceleration);
	}

}

void BehaviorSystem::separate(entityx::Entity& entity, entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> b1 = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Separation> separation = entity.component<sitara::ecs::Separation>();

	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	if(b1.valid() && separation.valid()) {
		entityx::ComponentHandle<sitara::ecs::DynamicBody> b2;
		for (auto e2 : entities.entities_with_components(b2)) {
			if (entity != e2) {
				ci::vec3 p1 = b1->getPosition();
				ci::vec3 p2 = b2->getPosition();
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < separation->mZoneRadius) {
					ci::vec3 desiredAcceleration = separation->mWeight * (10.0f / (distance)) * glm::normalize(offset); 
					b1->applyForce(desiredAcceleration);
				}
			}
		}
	}
}

void BehaviorSystem::cohere(entityx::Entity& entity, entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> b1 = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Cohesion> cohesion = entity.component<sitara::ecs::Cohesion>();
	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	if (b1.valid() && cohesion.valid()) {
		entityx::ComponentHandle<sitara::ecs::DynamicBody> b2;
		entityx::ComponentHandle<sitara::ecs::Cohesion> c2;
		ci::vec3 target(0,0,0);
		int entityCount = 0;
		for (auto e2 : entities.entities_with_components(b2, c2)) {
			if (entity != e2) {
				ci::vec3 p1 = b1->getPosition();
				ci::vec3 p2 = b2->getPosition();
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < cohesion->mZoneRadius) {
					target += b2->getPosition();
					entityCount++;
				}
			}
		}
		if (entityCount == 0) {
			target = ci::vec3(0, 0, 0);
		}
		else {
			target /= float(entityCount);
		}
		ci::vec3 position = b1->getPosition();
		ci::vec3 normalizedVelocity = ci::vec3(0);
		ci::vec3 targetOffset = target - position;
		if (glm::length(targetOffset) < 10) {
			normalizedVelocity = ci::vec3(0);
		}
		else {
			normalizedVelocity = glm::normalize(targetOffset);
		}
		ci::vec3 desiredVelocity = cohesion->mWeight * normalizedVelocity;
		ci::vec3 desiredAcceleration = desiredVelocity - b1->getVelocity();
		b1->applyForce(desiredAcceleration);
	}
}

void BehaviorSystem::align(entityx::Entity& entity, entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::DynamicBody> b1 = entity.component<sitara::ecs::DynamicBody>();
	entityx::ComponentHandle<sitara::ecs::Alignment> alignment = entity.component<sitara::ecs::Alignment>();

	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	if (b1.valid() && alignment.valid()) {
		entityx::ComponentHandle<sitara::ecs::DynamicBody> b2;
		ci::vec3 groupVelocity(0,0,0);
		for (auto e2 : entities.entities_with_components(b2)) {
			if (entity != e2) {
				ci::vec3 p1 = b1->getPosition();
				ci::vec3 p2 = b2->getPosition();
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < alignment->mZoneRadius) {
					groupVelocity += b2->getVelocity();
				}
			}
		}
		ci::vec3 normalizedVelocity = ci::vec3(0);
		if (glm::length(groupVelocity) == 0) {
			normalizedVelocity = ci::vec3(0);
		}
		else {
			normalizedVelocity = glm::normalize(groupVelocity);
		}
		ci::vec3 desiredVelocity = alignment->mWeight * normalizedVelocity;
		ci::vec3 desiredAcceleration = desiredVelocity - b1->getVelocity();
		b1->applyForce(desiredAcceleration);
	}
}
