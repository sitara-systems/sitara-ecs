#include "behavior/BehaviorSystem.h"
#include "behavior/StaticTarget.h"
#include "behavior/MovingTarget.h"
#include "behavior/NoiseField.h"
#include "behavior/Separation.h"
#include "behavior/Cohesion.h"
#include "behavior/Alignment.h"
#include "physics/RigidBody.h"
#include "cinder/app/App.h"

using namespace sitara::ecs;

void BehaviorSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::MovingTarget> movingTarget;
	entityx::ComponentHandle<sitara::ecs::StaticTarget> staticTarget;

	for (auto entity : entities.entities_with_components(movingTarget, staticTarget)) {
		staticTarget->mTargetPosition = physics::fromBtVector3(movingTarget->mRigidBody->getRigidBody()->getCenterOfMassPosition() + 
																movingTarget->mRigidBody->getRigidBody()->getLinearVelocity() * static_cast<float>(dt));
	}
}

void BehaviorSystem::seek(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::StaticTarget> target;

	for (auto entity : entities.entities_with_components(body, target)) {
		ci::vec3 position = physics::fromBtVector3(body->getRigidBody()->getCenterOfMassPosition());
		ci::vec3 desiredVelocity = target->mWeight * glm::normalize(target->mTargetPosition - position);
		ci::vec3 desiredAcceleration = desiredVelocity - physics::fromBtVector3(body->getRigidBody()->getLinearVelocity());
		body->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}
}

void BehaviorSystem::flee(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::StaticTarget> target;

	for (auto entity : entities.entities_with_components(body, target)) {
		ci::vec3 position = physics::fromBtVector3(body->getRigidBody()->getCenterOfMassPosition());
		ci::vec3 desiredVelocity = target->mWeight * glm::normalize(position - target->mTargetPosition);
		ci::vec3 desiredAcceleration = desiredVelocity - physics::fromBtVector3(body->getRigidBody()->getLinearVelocity());
		body->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}
}

void BehaviorSystem::arrive(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::StaticTarget> target;

	for (auto entity : entities.entities_with_components(body, target)) {
		ci::vec3 position = physics::fromBtVector3(body->getRigidBody()->getCenterOfMassPosition());
		ci::vec3 targetOffset = target->mTargetPosition - position;
		float distance = glm::length(targetOffset);
		ci::vec3 desiredVelocity;
		if (distance < target->mSlowingDistance) {
			// slow down
			desiredVelocity = target->mWeight * (distance / target->mSlowingDistance) * glm::normalize(target->mTargetPosition - position);
		}
		else {
			// regular seek behavior
			desiredVelocity = target->mWeight * glm::normalize(target->mTargetPosition - position);
		}
		ci::vec3 desiredAcceleration = desiredVelocity - physics::fromBtVector3(body->getRigidBody()->getLinearVelocity());
		body->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}
}

void BehaviorSystem::wander(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::NoiseField> noise;

	for (auto entity : entities.entities_with_components(body, noise)) {
		ci::vec3 position = physics::fromBtVector3(body->getRigidBody()->getCenterOfMassPosition());
		ci::vec3 direction = ci::vec3(
			Simplex::noise(ci::vec2(position.x + noise->mOffsets.x, ci::app::getElapsedSeconds() + noise->mOffsets.w)),
			Simplex::noise(ci::vec2(position.y + noise->mOffsets.y, ci::app::getElapsedSeconds() + noise->mOffsets.w)),
			Simplex::noise(ci::vec2(position.z + noise->mOffsets.z, ci::app::getElapsedSeconds() + noise->mOffsets.w))
		);
		ci::vec3 desiredAcceleration = noise->mWeight * glm::normalize(direction);
		body->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}

}

void BehaviorSystem::separate(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> b1;
	entityx::ComponentHandle<sitara::ecs::Separation> separation;

	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	for (auto e1 : entities.entities_with_components(b1, separation)) {
		entityx::ComponentHandle<sitara::ecs::RigidBody> b2;
		for (auto e2 : entities.entities_with_components(b2)) {
			if (e1 != e2) {
				ci::vec3 p1 = physics::fromBtVector3(b1->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 p2 = physics::fromBtVector3(b2->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < separation->mZoneRadius) {
					ci::vec3 desiredAcceleration = separation->mWeight * (1.0f / (distance)) * glm::normalize(offset);
					b1->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
				}
			}
		}
	}
}

void BehaviorSystem::cohere(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> b1;
	entityx::ComponentHandle<sitara::ecs::Cohesion> cohesion;

	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	for (auto e1 : entities.entities_with_components(b1, cohesion)) {
		entityx::ComponentHandle<sitara::ecs::RigidBody> b2;
		btVector3 target;
		int entityCount = 0;
		for (auto e2 : entities.entities_with_components(b2)) {
			if (e1 != e2) {
				ci::vec3 p1 = physics::fromBtVector3(b1->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 p2 = physics::fromBtVector3(b2->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < cohesion->mZoneRadius) {
					target += b2->getRigidBody()->getCenterOfMassPosition();
					entityCount++;
				}
			}
		}
		target /= entityCount;
		ci::vec3 position = physics::fromBtVector3(b1->getRigidBody()->getCenterOfMassPosition());
		ci::vec3 desiredVelocity = cohesion->mWeight * glm::normalize(physics::fromBtVector3(target) - position);
		ci::vec3 desiredAcceleration = desiredVelocity - physics::fromBtVector3(b1->getRigidBody()->getLinearVelocity());
		b1->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}
}

void BehaviorSystem::align(entityx::EntityManager& entities) {
	entityx::ComponentHandle<sitara::ecs::RigidBody> b1;
	entityx::ComponentHandle<sitara::ecs::Alignment> alignment;

	// this is implemented in an inefficient way; I should come back and find an elegant resolution with iterators
	for (auto e1 : entities.entities_with_components(b1, alignment)) {
		entityx::ComponentHandle<sitara::ecs::RigidBody> b2;
		btVector3 desiredVelocity;
		int entityCount = 0;
		for (auto e2 : entities.entities_with_components(b2)) {
			if (e1 != e2) {
				ci::vec3 p1 = physics::fromBtVector3(b1->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 p2 = physics::fromBtVector3(b2->getRigidBody()->getCenterOfMassPosition());
				ci::vec3 offset = p1 - p2;
				float distance = glm::length(offset);
				if (distance < alignment->mZoneRadius) {
					desiredVelocity += b2->getRigidBody()->getLinearVelocity();
					entityCount++;
				}
			}
		}
		desiredVelocity /= entityCount;
		desiredVelocity *= alignment->mWeight;
		ci::vec3 desiredAcceleration = physics::fromBtVector3(desiredVelocity - b1->getRigidBody()->getLinearVelocity());
		b1->getRigidBody()->applyCentralForce(physics::toBtVector3(desiredAcceleration));
	}
}