#pragma once

#include "PxPhysicsAPI.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class DynamicBody {
		public:
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
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxBoxGeometry(halfEdge, halfEdge, halfEdge), *material);
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

			void setLocalPose(const ci::quat& quat, const ci::vec3& axis) {
				mShape->setLocalPose(sitara::ecs::physics::to(quat, axis));
			}

			void applyForce(const ci::vec3& acceleration) {
				mBody->addForce(sitara::ecs::physics::to(acceleration));
			}

			void resetBody(const ci::vec3& position, const ci::quat& rotation = ci::quat()) {
				physx::PxVec3 nullVelocity = physx::PxVec3(0);
				physx::PxTransform nullTransform = sitara::ecs::physics::to(rotation, position);

				mBody->setLinearVelocity(nullVelocity);
				mBody->setAngularVelocity(nullVelocity);
				mBody->setGlobalPose(nullTransform, true);
				mBody->clearForce();
				mBody->clearTorque();
			}

			bool isSleeping() {
				return mBody->isSleeping();
			}
			
			void setName(const std::string& name) {
				mBody->setName(name.c_str());
			}

			std::string getName() {
				return mBody->getName();
			}

		protected:
			physx::PxRigidDynamic* mBody;
			physx::PxShape* mShape;

			friend class PhysicsSystem;
		};
	}
}
