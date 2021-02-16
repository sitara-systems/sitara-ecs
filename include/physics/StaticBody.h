#pragma once

#include "PxPhysicsAPI.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class StaticBody {
		public:
			StaticBody(physx::PxRigidStatic* staticBody) {
				mBody = staticBody;
				mShape = nullptr;
				mIsDirty = true;
			}

			~StaticBody() {
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
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxBoxGeometry(sitara::ecs::physics::to(halfEdges)), *material);
			}

			const ci::vec3 getPosition() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().p);
			}

			const ci::quat getRotation() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().q);
			}

			void setLocalPose(const ci::quat& quat, const ci::vec3& axis) {
				mShape->setLocalPose(sitara::ecs::physics::to(quat, axis));
				mIsDirty = true;
			}

		protected:
			bool isDirty() {
				return mIsDirty;
			}

			void setDirty(bool dirty) {
				mIsDirty = dirty;
			}

			physx::PxRigidStatic* mBody;
			physx::PxShape* mShape;
			bool mIsDirty;

			friend class PhysicsSystem;
		};
	}
}
