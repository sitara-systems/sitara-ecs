#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {

		class GhostBody {
		public:
			GhostBody(btGhostObject* body) {
				mGhostObject = body;
				mCollisionState = false;
				mLastCollisionState = true;
			}

			~GhostBody() {
				if (mGhostObject != 0) {
					delete mGhostObject;
				}
				if (mCollisionShape != 0) {
					delete mCollisionShape;
				}
			}

			static btGhostObject* createBox(const ci::vec3& size, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btVector3 halfSize = physics::toBtVector3(size) * 0.5f;
				btCollisionShape* shape = new btBoxShape(halfSize);
				btGhostObject* body = createGhostObject(shape, position, rotation);
				return body;
			}

			static btGhostObject* createSphere(float radius, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btCollisionShape* shape = new btSphereShape((btScalar)radius);
				btGhostObject* body = createGhostObject(shape, position, rotation);
				return body;
			}

			static btGhostObject* createCone(float radius, float height, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btCollisionShape* shape = new btConeShape(radius, height);
				btGhostObject* body = createGhostObject(shape, position, rotation);
				return body;
			}

			static btGhostObject* createCylinder(float radius, float height, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btCollisionShape* shape = new btCylinderShape(physics::toBtVector3(ci::vec3(radius, radius, 0.5f * height)));
				btGhostObject* body = createGhostObject(shape, position, rotation);
				return body;
			}

			static btGhostObject* createCapsule(float radius, float height, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btCollisionShape* shape = new btCapsuleShape(radius, height);
				btGhostObject* body = createGhostObject(shape, position, rotation);
				return body;
			}

			btGhostObject* getGhostBody() const {
				return mGhostObject;
			}

			void setCollisionState(bool collision) {
				mLastCollisionState = mCollisionState;
				mCollisionState = collision;
			}

			void addOnEnterGhostCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mOnEnterGhostCollisionFns.push_back(callback);
			}

			void addDuringGhostCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mDuringGhostCollisionFns.push_back(callback);
			}

			void addOnEndGhostCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mOnEndGhostCollisionFns.push_back(callback);
			}

			void applyCollisionFunctions(entityx::ComponentHandle<sitara::ecs::RigidBody> rigidBody) {
				if (mCollisionState && !mLastCollisionState) {
					// entering collision
					for (auto& callback : mOnEnterGhostCollisionFns) {
						callback(rigidBody);
					}
				}
				else if (mCollisionState && mLastCollisionState) {
					// during collision
					for (auto& callback : mDuringGhostCollisionFns) {
						callback(rigidBody);
					}
				}
				else if ((!mCollisionState && mLastCollisionState)) {
					// exiting collision
					for (auto& callback : mOnEndGhostCollisionFns) {
						callback(rigidBody);
					}
				}
			}

		protected:
			static btGhostObject* createGhostObject(btCollisionShape* shape, const ci::vec3& position = ci::vec3(0), const ci::quat& rotation = ci::quat()) {
				btQuaternion btRotation = physics::toBtQuaternion(rotation);
				btVector3 btPosition = physics::toBtVector3(position);
				btTransform worldTransform(btRotation, btPosition);

				btGhostObject* body = new btGhostObject();
				body->setWorldTransform(worldTransform);
				body->setCollisionShape(shape);
				body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

				return body;
			}

			bool mCollisionState;
			bool mLastCollisionState;
			btCollisionShape* mCollisionShape;
			btGhostObject* mGhostObject;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mOnEnterGhostCollisionFns;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mDuringGhostCollisionFns;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mOnEndGhostCollisionFns;
		};
	}
}
