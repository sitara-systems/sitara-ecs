#pragma once

#include <map>
#include "entityx/System.h"
#include "cinder/Vector.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxExtensionsAPI.h"
#include "physics/DynamicBody.h"
#include "physics/StaticBody.h"
#include "physics/OverlapDetector.h"

PX_C_EXPORT bool PX_CALL_CONV PxInitExtensions(physx::PxPhysics& physics, physx::PxPvd* pvd);

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
			void setGravity(const ci::vec3& gravity);
			void setNumberOfThreads(const uint32_t numThreads);
			void enableGpu(const bool enable);
			physx::PxRigidStatic* createStaticBody(const ci::vec3& position, const ci::quat& rotation = ci::quat());
			physx::PxRigidDynamic* createDynamicBody(const ci::vec3& position, const ci::quat& rotation = ci::quat());
			physx::PxDistanceJoint* createSpring(entityx::ComponentHandle<sitara::ecs::DynamicBody> body, ci::vec3 anchorPoint, float stiffness, float dampingConstant);
			int registerMaterial(const float staticFriction, const float dynamicFriction, const float restitution);
			physx::PxMaterial* getMaterial(const int materialId);
			//int registerShape(const float staticFriction, const float dynamicFriction, const float restitution);
			//physx::PxShape* getShape(const int shapeId);
			void addPreUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> callback);
			void addPostUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> callback);
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
			float mSimulationTime;
			std::map<int, physx::PxMaterial*> mMaterialRegistry;
			uint32_t mMaterialCount;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> > mPreUpdateFns;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::DynamicBody>)> > mPostUpdateFns;
		};
	}
}
