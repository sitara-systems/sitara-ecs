#pragma once

#include <map>
#include "entityx/System.h"
#include "cinder/Vector.h"
#include "PxPhysicsAPI.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "physics/DynamicBody.h"
#include "physics/StaticBody.h"

namespace sitara {
	namespace ecs {

		class PhysicsSystem : public entityx::System<PhysicsSystem>, public entityx::Receiver<PhysicsSystem> {
		public:
			PhysicsSystem();
			~PhysicsSystem();
			void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
			void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
			void receive(const entityx::ComponentAddedEvent<DynamicBody>& event);
			void receive(const entityx::ComponentRemovedEvent<DynamicBody>& event);
			void receive(const entityx::ComponentAddedEvent<StaticBody>& event);
			void receive(const entityx::ComponentRemovedEvent<StaticBody>& event);
			double getElapsedSimulationTime();
			void setGravity(ci::vec3& gravity);
			void setNumberOfThread(uint32_t numThreads);
			void enableGpu(bool enable);
			physx::PxRigidStatic* createStaticBody(ci::vec3& position, ci::quat& rotation);
			physx::PxRigidDynamic* createDynamicBody(ci::vec3& position, ci::quat& rotation);
			int registerMaterial(float staticFriction, float dynamicFriction, float restitution);
			physx::PxMaterial* getMaterial(int materialId);
		private:
			physx::PxDefaultAllocator mAllocator;
			physx::PxDefaultErrorCallback mErrorCallback;
			physx::PxFoundation* mFoundation;
			physx::PxPhysics* mPhysics;
			physx::PxDefaultCpuDispatcher* mDispatcher;
			physx::PxCudaContextManager* mCudaContext;
			physx::PxScene* mScene;
			physx::PxPvd* mPvd;
			bool mGpuEnabled;
			uint32_t mNumberOfThreads;
			std::map<int, physx::PxMaterial*> mMaterialRegistry;
			uint32_t mMaterialCount;
		};
	}
}
