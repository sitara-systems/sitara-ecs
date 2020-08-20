#pragma once

#include <map>
#include "entityx/System.h"
#include "cinder/Vector.h"
#include "bullet/btBulletDynamicsCommon.h"
#include "physics/RigidBody.h"
#include "physics/GhostBody.h"

namespace sitara {
	namespace ecs {
		class PhysicsSystem : public entityx::System<PhysicsSystem>, public entityx::Receiver<PhysicsSystem> {
		public:
			PhysicsSystem();
			~PhysicsSystem();
			void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
			void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
			void receive(const entityx::ComponentAddedEvent<RigidBody>& event);
			void receive(const entityx::ComponentRemovedEvent<RigidBody>& event);
			void receive(const entityx::ComponentAddedEvent<GhostBody>& event);
			void receive(const entityx::ComponentRemovedEvent<GhostBody>& event);
			double getElapsedSimulationTime();
			bool setGhostCollisions(bool enable);
			void setGravity(ci::vec3 gravity);
			void clearForces(entityx::EntityManager& entities);
			void resetBody(entityx::ComponentHandle<sitara::ecs::RigidBody> body, ci::vec3 position);
			void addRigidBodyUpdateFn(std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> callback);
			btDiscreteDynamicsWorld* getWorld();
		private:
			void checkGhostBodyCollisions();
			btDefaultCollisionConfiguration* mCollisionConfiguration;
			btCollisionDispatcher* mDispatcher;
			btBroadphaseInterface* mOverlappingPairCache;
			btSequentialImpulseConstraintSolver* mBulletSolver;
			btDiscreteDynamicsWorld* mDynamicsWorld;
			double mElapsedSimulationTime;
			bool mEnableGhostCollisions;
			std::vector<btGhostObject*> mActiveGhostObjects;
			std::vector<btCollisionObjectArray> mGhostCollisions;
			std::map<btRigidBody*, entityx::ComponentHandle<sitara::ecs::RigidBody>, std::less<btRigidBody*>> mRigidBodyMap;
			std::map<btGhostObject*, entityx::ComponentHandle<sitara::ecs::GhostBody>, std::less<void>> mGhostBodyMap;
			std::vector<std::function<void(entityx::ComponentHandle<sitara::ecs::RigidBody>)> > mRigidBodyUpdateFns;
		};
	}
}
