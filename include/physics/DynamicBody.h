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
			}

			~DynamicBody() {
			}

			physx::PxRigidDynamic* getBody() {
				return mBody;
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

			void applyForce(ci::vec3 acceleration) {
				mBody->addForce(sitara::ecs::physics::to(acceleration));
			}

		protected:
			physx::PxRigidDynamic* mBody;
			physx::PxShape* mShape;
		};
	}
}
