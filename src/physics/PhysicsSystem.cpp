#include "PhysicsSystem.h"
#include "PhysicsBody.h"

using namespace sitara::ecs;

void PhysicsSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
  entityx::ComponentHandle<PhysicsBody> body;
  for(auto e : entities.entities_with_components(body)) {
    auto &b = *body.get();
    auto current = b.mPosition;
    auto velocity = (b.mPosition - b.mPreviousPosition) * static_cast<float>((dt / mPreviousDelta)) + b.mAcceleration * static_cast<float>(dt * dt);
	if (glm::length(velocity) > mMaxSpeed) {
		auto dir = glm::normalize(velocity);
		velocity = mMaxSpeed * dir;
	}
    //velocity *= (1.0 - b.mDrag); // Friction as viscous drag.
    b.mPosition += velocity;
    b.mPreviousPosition = current;
    b.mAcceleration = ci::vec3(0); // clear acceleration
    mPreviousDelta = dt;
  }
}	
