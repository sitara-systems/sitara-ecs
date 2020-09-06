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

			/*
			Event Callbacks -- each takes two arguments; the GhostBody Component Handle referring to this object, and the RigidBody Component Handle referring to the other object
			that collided with our GhostBody.
			*/
			void addOnEnterEachCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mOnEnterEachCollisionFns.push_back(callback);
			}

			void addDuringEachCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mDuringEachCollisionFns.push_back(callback);
			}

			void addOnEndEachCollisionFn(std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback) {
				mOnEndEachCollisionFns.push_back(callback);
			}

			void applyCollisionFunctions(entityx::ComponentHandle<sitara::ecs::GhostBody> thisGhostComponent) {
				for (auto& otherRigidBodyComponent : mRigidBodyCollisions) {
					if (std::find(mLastCollisionFrameObjects.begin(), mLastCollisionFrameObjects.end(), otherRigidBodyComponent) != mLastCollisionFrameObjects.end()) {
						// in current collisions + previous collisions, still colliding
						for (auto& callback : mOnEnterEachCollisionFns) {
							callback(thisGhostComponent, otherRigidBodyComponent);
						}
					}
					else {
						// in current collision but NOT previous collision, starting collision
						for (auto& callback : mDuringEachCollisionFns) {
							callback(thisGhostComponent, otherRigidBodyComponent);
						}
					}
				}
				for (auto& otherRigidBodyComponent : mLastCollisionFrameObjects) {
					if (std::find(mRigidBodyCollisions.begin(), mRigidBodyCollisions.end(), otherRigidBodyComponent) == mRigidBodyCollisions.end()) {
						// in previous collisions + NOT in current collisions, ending collision
						for (auto& callback : mOnEndEachCollisionFns) {
							callback(thisGhostComponent, otherRigidBodyComponent);
						}
					}
				}
			}

			void resetCollisionBodies() {
				mLastCollisionFrameObjects.clear();
				std::copy(mRigidBodyCollisions.begin(), mRigidBodyCollisions.end(), std::back_inserter(mLastCollisionFrameObjects));
				mRigidBodyCollisions.clear();
			}

			void addCollisionBodies(entityx::ComponentHandle<sitara::ecs::RigidBody> body) {
				mRigidBodyCollisions.push_back(body);
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
			std::vector<entityx::ComponentHandle<sitara::ecs::RigidBody>> mRigidBodyCollisions;
			std::vector<entityx::ComponentHandle<sitara::ecs::RigidBody>> mLastCollisionFrameObjects;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mOnEnterEachCollisionFns;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mDuringEachCollisionFns;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::GhostBody>, entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mOnEndEachCollisionFns;
		};
	}
}
