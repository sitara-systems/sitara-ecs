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
				mCollisionShape(shape)
			{
				btTransform localTransform;
				localTransform.setIdentity();
				localTransform.setOrigin(btVector3(position.x, position.y, position.z));

				btScalar btMass(mass);
				bool isDynamic = (mass != 0.f); //rigidbody is dynamic if and only if mass is non zero, otherwise static
				btVector3 localInertia(inertia.x, inertia.y, inertia.z);

				std::shared_ptr<btCollisionShape> collisionShape;
				switch (shape) {
					case SPHERE:
						collisionShape = std::make_shared<btSphereShape>(btScalar(size.x));
						break;
					case BOX:
						collisionShape = std::make_shared<btBoxShape>(btVector3(btScalar(size.x), btScalar(size.y), btScalar(size.z)));
						break;
				}


				if (isDynamic) {
					collisionShape->calculateLocalInertia(mass, localInertia);
				}

				std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>(localTransform);

				btRigidBody::btRigidBodyConstructionInfo info(btMass, motionState.get(), collisionShape.get(), localInertia);
				mRigidBody = std::make_shared<btRigidBody>(info);
			}

			const ci::vec3 getPosition() const {
				btTransform trans;
				if (mRigidBody->getMotionState()) {
					mRigidBody->getMotionState()->getWorldTransform(trans);
				}
				return ci::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			}

			const std::shared_ptr<btRigidBody> getRigidBody() const {
				return mRigidBody;
			}

		private:
			std::shared_ptr<btRigidBody> mRigidBody;
			sitara::ecs::SHAPE mCollisionShape;
		};
	}
}
