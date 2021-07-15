#pragma once

#include "PxPhysicsAPI.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class DynamicBody {
		public:
			DynamicBody() {
				mBody = nullptr;
				mShape = nullptr;
			}

			DynamicBody(physx::PxRigidDynamic* DynamicBody) {
				mBody = DynamicBody;
				mShape = nullptr;
			}

			~DynamicBody() {
				if (mBody) {
					mBody->release();
				}
			}

			void attachShape(physx::PxShape* shape) {
				mBody->attachShape(*shape);
				mShape = shape;
			}

			void detachShape() {
				if (mShape) {
					mBody->detachShape(*mShape);
					mShape = nullptr;
				}
			}

			physx::PxGeometryType::Enum getGeometryType() {
				if (mShape) {
					return mShape->getGeometryType();
				}
				else {
					return physx::PxGeometryType::Enum::eINVALID;
				}
			}

			void attachSphere(float radius, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxSphereGeometry(radius), *material);
			}

			void attachCapsule(float radius, float halfHeight, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxCapsuleGeometry(radius, halfHeight), *material);
				physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));
				mShape->setLocalPose(relativePose);
			}

			void attachBox(float halfEdge, physx::PxMaterial* material) {
				attachBox(ci::vec3(halfEdge), material);
			}

			void attachBox(const ci::vec3& halfEdges, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxBoxGeometry(halfEdges.x, halfEdges.y, halfEdges.z), *material);
			}

			const ci::vec3 getPosition() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().p);
			}

			const ci::quat getRotation() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().q);
			}

			const ci::vec3 getVelocity() {
				return sitara::ecs::physics::from(mBody->getLinearVelocity());
			}

			const ci::vec3 getAngularVelocity() {
				return sitara::ecs::physics::from(mBody->getAngularVelocity());
			}

			void setMass(float mass) {
				mBody->setMass(mass);
			}

			void setLocalPose(const ci::quat& quat) {
				mShape->setLocalPose(physx::PxTransform(sitara::ecs::physics::to(quat)));
			}

			void applyForce(const ci::vec3& acceleration) {
				mBody->addForce(sitara::ecs::physics::to(acceleration));
			}

			void clearForces() {
				mBody->clearForce();
				mBody->clearTorque();
			}

			void resetBody(const ci::vec3& position, const ci::vec3& velocity = ci::vec3(), const ci::quat& rotation = ci::quat(), const ci::vec3& angularVelocity = ci::vec3()) {
				physx::PxTransform nullTransform = sitara::ecs::physics::to(rotation, position);

				mBody->setLinearVelocity(sitara::ecs::physics::to(velocity));
				mBody->setAngularVelocity(sitara::ecs::physics::to(angularVelocity));
				mBody->setGlobalPose(nullTransform, true);
				mBody->clearForce();
				mBody->clearTorque();
			}

			bool isSleeping() {
				return mBody->isSleeping();
			}

			void setUserData(uint64_t userData) {
				mBody->userData = (void*)userData;
			}

			uint64_t getUserData() {
				return (uint64_t)mBody->userData;
			}
			
		protected:
			physx::PxRigidDynamic* mBody;
			physx::PxShape* mShape;

			friend class PhysicsSystem;
		};
	}
}
