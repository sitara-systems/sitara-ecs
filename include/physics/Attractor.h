#pragma once

#include "Particle.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class Attractor {
		public:
			Attractor(const ci::vec3& position, const ci::vec3& forceStrength) {
				mPosition = position;
				mForceStrength = forceStrength;
			}

			void apply(entityx::ComponentHandle<Particle> particle) {
				float distanceSq = glm::distance(mPosition, particle->getPosition());
				distanceSq *= distanceSq;

				float forceConstant = particle->getMass() / distanceSq;

				ci::vec3 unitVector = mPosition - particle->getPosition();
				glm::normalize(unitVector);

				unitVector[0] *= forceConstant * mForceStrength[0];
				unitVector[1] *= forceConstant * mForceStrength[1];
				unitVector[2] *= forceConstant * mForceStrength[2];

				particle->addForce(unitVector);
			}

			void setPosition(ci::vec3 position) {
				mPosition = position;
			}

			ci::vec3 getPosition() {
				return mPosition;
			}

			void setStrength(const ci::vec3& strength) {
				mForceStrength = strength;
			}

			void turnOn() {
				mIsOn = true;
			}

			void turnOff() {
				mIsOn = false;
			}

			bool IsOn() {
				return mIsOn;
			}

		protected:
			ci::vec3 mPosition;
			ci::vec3 mForceStrength;
			bool mIsOn;
		};
	}
}