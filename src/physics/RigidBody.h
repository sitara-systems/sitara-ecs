#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "bullet/btBulletDynamicsCommon.h"

namespace sitara {
	namespace ecs {
		enum SHAPE {
			SPHERE,
			BOX
		};

		class RigidBody {
		public:
			RigidBody::RigidBody() = default;
			RigidBody::RigidBody(sitara::ecs::SHAPE shape, ci::vec3 position = ci::vec3(0.0f), ci::vec3 size = ci::vec3(1.0f), float mass = 0.0f, ci::vec3 inertia = ci::vec3(0.0f)) :
				mShape(shape)
			{
				btTransform localTransform;
				localTransform.setIdentity();
				localTransform.setOrigin(btVector3(position.x, position.y, position.z));

				btScalar btMass(mass);
				bool isDynamic = (mass != 0.f); //rigidbody is dynamic if and only if mass is non zero, otherwise static
				btVector3 localInertia(inertia.x, inertia.y, inertia.z);

				btCollisionShape* mCollisionShape;
				switch (shape) {
					case SPHERE:
						mCollisionShape = new btSphereShape(btScalar(size.x));
						break;
					case BOX:
						mCollisionShape = new btBoxShape(btVector3(btScalar(size.x), btScalar(size.y), btScalar(size.z)));
						break;
				}


				if (isDynamic) {
					mCollisionShape->calculateLocalInertia(mass, localInertia);
				}

				mMotionState = new btDefaultMotionState(localTransform);

				btRigidBody::btRigidBodyConstructionInfo info(btMass, mMotionState, mCollisionShape, localInertia);
				mRigidBody = new btRigidBody(info);
			}

			RigidBody::~RigidBody() {
				delete mCollisionShape;
				delete mMotionState;
				delete mRigidBody;
			}

			ci::vec3 getPosition() {
				btTransform trans;
				if (mRigidBody->getMotionState()) {
					mRigidBody->getMotionState()->getWorldTransform(trans);
				}
				return ci::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			}

			btRigidBody* getRigidBody() {
				return mRigidBody;
			}

		private:
			btCollisionShape* mCollisionShape;
			btDefaultMotionState* mMotionState;
			btRigidBody* mRigidBody;
			sitara::ecs::SHAPE mShape;
		};
	}
}
