#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"

namespace sitara {
	namespace ecs {
		class Particle {
		public:
			Particle(float mass) {
				mMass = mass;
				mPosition = ci::vec3(0);
				reset();
			}

			Particle(float mass, float x, float y, float z) {
				mMass = mass;
				mPosition = ci::vec3(x, y, z);
				reset();
			}

			Particle(float mass, const ci::vec3& position) {
				mMass = mass;
				mPosition = position;
				reset();
			}

			void setMass(float mass) {
				mMass = mass;
			}

			float getMass() {
				return mMass;
			}

			void makeFixed() {
				mIsFree = false;
			}

			void makeFree() {
				mIsFree = true;
			}

			bool isFree() {
				return mIsFree;
			}

			bool isAlive() {
				return mIsAlive;
			}

			int getAge() {
				return mAge;
			}

			ci::vec3& getPosition() {
				return mPosition;
			}

			ci::vec3& getVelocity() {
				return mVelocity;
			}

			void addForce(const ci::vec3& force) {
				mForces = mForces + force;
			}

			ci::vec3& getForces() {
				return mForces;
			}

			void reset(const ci::vec3& position = ci::vec3(0), const ci::vec3& velocity = ci::vec3(0)) {
				mVelocity = ci::vec3(0);
				mForces = ci::vec3(0);
				mAge = -1;
				mIsAlive = true;
				mIsFree = true;
			}

		protected:

			void setPosition(const ci::vec3& position) {
				mPosition = position;
			}

			void setVelocity(const ci::vec3& velocity) {
				mVelocity = velocity;
			}

			void clearForces() {
				mForces = ci::vec3(0);
			}

			void setAge(int age) {
				mAge = age;
			}

			ci::vec3 mPosition;
			ci::vec3 mVelocity;
			ci::vec3 mForces;
			float mMass;
			int mAge;
			bool mIsAlive;
			bool mIsFree;

			friend class ParticleSystem;
		};
	}
}
