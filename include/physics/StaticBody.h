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
			}

			~StaticBody() {
			}

			void attachSphere(float radius, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxSphereGeometry(radius), *material);
			}

			void attachCapsule(float radius, float halfHeight, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxCapsuleGeometry(radius, halfHeight), *material);
			}

			void attachBox(float halfEdge, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxBoxGeometry(halfEdge, halfEdge, halfEdge), *material);
			}

			void attachBox(ci::vec3 halfEdges, physx::PxMaterial* material) {
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxBoxGeometry(halfEdges.x, halfEdges.y, halfEdges.z), *material);
			}

			void attachPlane(ci::vec3 normal, float distance, physx::PxMaterial* material) {
				// needs tweaking here
				mShape = physx::PxRigidActorExt::createExclusiveShape(*mBody, physx::PxPlaneGeometry(), *material);
			}

			const ci::vec3 getPosition() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().p);
			}

			const ci::quat getRotation() {
				return sitara::ecs::physics::from(mBody->getGlobalPose().q);
			}

			void setLocalPose(const ci::quat& quat, const ci::vec3& axis) {
				mShape->setLocalPose(sitara::ecs::physics::to(quat, axis));
			}

		protected:
			physx::PxRigidStatic* mBody;
			physx::PxShape* mShape;
		};
	}
}
