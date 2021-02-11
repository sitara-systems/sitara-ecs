#pragma once

#include "PxPhysicsAPI.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class OverlapDetector {
		public:
			OverlapDetector(const ci::vec3& center, float OverlapDistance) : mCurrentResults(),
				mPreviousResults(),
				mQueryFilter(),
				mTransform(sitara::ecs::physics::to(ci::quat(), center)) {
				mOverlapShape = new physx::PxSphereGeometry(OverlapDistance);
				mCurrentResults.reserve(4096);
				mPreviousResults.reserve(4096);
			}

			~OverlapDetector() {
				delete mOverlapShape;
			}

			void queryAll() {
				mQueryFilter.flags = physx::PxQueryFlag::eSTATIC;
				mQueryFilter.flags |= physx::PxQueryFlag::eDYNAMIC;
			}

			void queryStaticOnly() {
				mQueryFilter.flags = physx::PxQueryFlag::eSTATIC;
			}

			void queryDynamicOnly() {
				mQueryFilter.flags = physx::PxQueryFlag::eDYNAMIC;
			}

			void addOnEnterEachCollisionFn(std::function<void(entityx::Entity e1, entityx::Entity e2)> callback) {
				mOnEnterEachCollisionFns.push_back(callback);
			}

			void addDuringEachCollisionFn(std::function<void(entityx::Entity e1, entityx::Entity e2)> callback) {
				mDuringEachCollisionFns.push_back(callback);
			}

			void addOnEndEachCollisionFn(std::function<void(entityx::Entity e1, entityx::Entity e2)> callback) {
				mOnEndEachCollisionFns.push_back(callback);
			}

		protected:
			physx::PxGeometry& getGeometry() {
				return *mOverlapShape;
			}

			physx::PxTransform& getTransform() {
				return mTransform;
			}

			physx::PxOverlapHit* getWriteBuffer() {
				return mCurrentResults.data();
			}

			int getBufferSize() {
				return mBufferSize;
			}

			void resizeBuffer(size_t size) {
				mCurrentResults.resize(size);
			}

			void swapBuffers() {
				mCurrentResults.swap(mPreviousResults);
				mCurrentResults.clear();
			}

			std::vector<physx::PxOverlapHit>& getResults() {
				return mCurrentResults;
			}

			std::vector<physx::PxOverlapHit>& getPreviousResults() {
				return mPreviousResults;
			}

			physx::PxQueryFilterData& getFilter() {
				return mQueryFilter;
			}

			static const int mBufferSize = 4096;
			physx::PxGeometry* mOverlapShape;
			physx::PxTransform mTransform;
			physx::PxQueryFilterData mQueryFilter;
			
			std::vector<physx::PxOverlapHit> mCurrentResults;
			std::vector<physx::PxOverlapHit> mPreviousResults;

			std::vector<std::function<void(entityx::Entity e1, entityx::Entity e2)> > mOnEnterEachCollisionFns;
			std::vector<std::function<void(entityx::Entity e1, entityx::Entity e2)> > mDuringEachCollisionFns;
			std::vector<std::function<void(entityx::Entity e1, entityx::Entity e2)> > mOnEndEachCollisionFns;

			friend class PhysicsSystem;
		};
	}
}
