#pragma once

#include "Particle.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class Spring {
		public:
			Spring(entityx::ComponentHandle<Particle> particle, const ci::vec3& anchor, float spring_constant, float damping, float length) {
				mParticle = particle;
				mAnchorPosition = anchor;
				mSpringConstant = spring_constant;
				mDamping = damping;
				mRestLength = length;
			}

			float getCurrentLength() {
				return glm::distance(mParticle->getPosition(), mAnchorPosition);
			}

			float getRestLength() {
				return mRestLength;
			}

			float getSpringConstant() {
				return mSpringConstant;
			}

			float getDampingConstant() {
				return mDamping;
			}

			void setAnchorPosition(const ci::vec3& anchor) {
				mAnchorPosition = anchor;
			}

			void setLength(float length) {
				mRestLength = length;
			}

			void setSpringConstant(float k) {
				mSpringConstant = k;
			}

			void setDamping(float d) {
				mDamping = d;
			}

			void setParticle(entityx::ComponentHandle<Particle> particle) {
				mParticle = particle;
			}

			void apply() {
				/*
				* Get the unit vector that points along the spring axis
				*/
				ci::vec3 direction = mParticle->getPosition() - mAnchorPosition;
				if (direction == ci::vec3(0)) {
					// if this is ever exactly zero, then the physics breaks because the direction is zero
					direction = ci::randVec3();
				}
				direction = glm::normalize(direction);

				/*
				*  Compute restorative spring force
				*  Proportional to how much the string is stretched past its rest length
				*/

				float distance = getCurrentLength();
				float springMagnitude = -mSpringConstant * (distance - mRestLength);

				/*
				*  Compute damping force
				*  proportional to the velocity along the spring axis
				*/
				ci::vec3 velocityDirection = mParticle->getVelocity();
				if (velocityDirection == ci::vec3(0)) {
					// if this is ever exactly zero, then the physics breaks because the direction is zero
					velocityDirection = ci::randVec3();
				}
				velocityDirection = glm::normalize(velocityDirection);
				float dot = glm::dot(velocityDirection, direction);
				float dampingMagnitude = -mDamping * dot;

				mSpringForce = springMagnitude * direction;
				mDampingForce = dampingMagnitude * direction;
				ci::vec3 totalForce = (springMagnitude + dampingMagnitude) * direction;

				mParticle->addForce(totalForce);
			}

			ci::vec3 getSpringForce() {
				return mSpringForce;
			}

			ci::vec3 getDampingForce() {
				return mDampingForce;
			}

			ci::vec3 getAnchorPosition() {
				return mAnchorPosition;
			}

		protected:
			entityx::ComponentHandle<Particle> mParticle;
			ci::vec3 mAnchorPosition;
			ci::vec3 mSpringForce;
			ci::vec3 mDampingForce;
			float mRestLength;
			float mDamping;
			float mSpringConstant;
		};
	}
}
