#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "physics/PhysicsUtilities.h"

namespace sitara {
	namespace ecs {
		enum SHAPE {
			SPHERE,
			BOX
		};

		class RigidBody {
		public:
			RigidBody::RigidBody() = default;
			RigidBody::RigidBody(sitara::ecs::SHAPE shape, 
									ci::vec3 position = ci::vec3(0.0f), 
									ci::vec3 size = ci::vec3(1.0f), 
									float mass = 0.0f, 
									ci::vec3 inertia = ci::vec3(0.0f)) :
				mShape(shape)
			{
				btTransform localTransform;
				localTransform.setIdentity();
				localTransform.setOrigin(physics::toBtVector3(position));

				btScalar btMass(mass);
				bool isDynamic = (mass != 0.f); //rigidbody is dynamic if and only if mass is non zero, otherwise static
				btVector3 localInertia(physics::toBtVector3(inertia));
		
				btCollisionShape* mCollisionShape;
				switch (shape) {
					case SPHERE:
						mCollisionShape = new btSphereShape(size.x);
						break;
					case BOX:
						mCollisionShape = new btBoxShape(physics::toBtVector3(size));
						break;
				}


				if (isDynamic) {
					mCollisionShape->calculateLocalInertia(mass, localInertia);
				}

				mMotionState = new btDefaultMotionState(localTransform);

				btRigidBody::btRigidBodyConstructionInfo info(btMass, mMotionState, mCollisionShape, localInertia);
				info.m_restitution = 0.1;
				info.m_friction = 0.1;

				mRigidBody = new btRigidBody(info);
			}

			RigidBody::~RigidBody() {
				delete mRigidBody;
				delete mMotionState;
				delete mCollisionShape;
			}

			void resetBody(ci::vec3 position) {
				if (mRigidBody->getMotionState()) {
					btTransform transform;
					transform.setIdentity();
					transform.setOrigin(physics::toBtVector3(position));
					transform.setRotation(btQuaternion());

					mRigidBody->setWorldTransform(transform);
					mRigidBody->getMotionState()->setWorldTransform(transform);

					mRigidBody->setLinearVelocity(btVector3(0, 0, 0));
					mRigidBody->setAngularVelocity(btVector3(0, 0, 0));
					mRigidBody->clearForces();
				}
			}

			ci::vec3 getPosition() {
				btTransform trans;
				if (mRigidBody->getMotionState()) {
					mRigidBody->getMotionState()->getWorldTransform(trans);
				}
				return physics::fromBtVector3(trans.getOrigin());
			}

			ci::mat4 getWorldTransform() {
				btTransform trans;
				if (mRigidBody->getMotionState()) {
					mRigidBody->getMotionState()->getWorldTransform(trans);
				}
				return physics::fromBtTransform(trans);
			}

			btRigidBody* getRigidBody() const {
				return mRigidBody;
			}

			btRigidBody* setElasticity(float elasticity) {
				mRigidBody->setRestitution(ci::clamp<float>(elasticity, 0.0, 1.0));
				return mRigidBody;
			}

			btRigidBody* setFriction(float friction) {
				mRigidBody->setFriction(ci::clamp<float>(friction, 0.0, 1.0));
				return mRigidBody;
			}

			void setLinearVelocity(ci::vec3 velocity) {
				mRigidBody->setLinearVelocity(physics::toBtVector3(velocity));
			}

		private:
			btCollisionShape* mCollisionShape;
			btDefaultMotionState* mMotionState;
			btRigidBody* mRigidBody;
			sitara::ecs::SHAPE mShape;
		};
	}
}
