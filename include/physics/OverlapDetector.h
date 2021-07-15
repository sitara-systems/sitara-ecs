#pragma once

#include "PxPhysicsAPI.h"
#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "physics/PhysicsUtils.h"

namespace sitara {
	namespace ecs {
		class OverlapDetector {
		public:
			OverlapDetector(const ci::vec3& center, float overlapDistance) : mCurrentResults(NULL),
				mPreviousResults(NULL),
				mQueryFilter(),
				mTransform(sitara::ecs::physics::to(ci::quat(), center)) {
				mOverlapShape = new physx::PxSphereGeometry(overlapDistance);
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

			void addOnEnterEachOverlapFn(std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> callback) {
				mOnEnterEachOverlapFns.push_back(callback);
			}

			void addDuringEachOverlapFn(std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> callback) {
				mDuringEachOverlapFns.push_back(callback);
			}

			void addOnEndEachOverlapFn(std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> callback) {
				mOnEndEachOverlapFns.push_back(callback);
			}

		protected:
			physx::PxGeometry& getGeometry() {
				return *mOverlapShape;
			}

			void setTransform(const physx::PxTransform& transform) {
				mTransform = transform;
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

			void saveResults() {
				mPreviousResults = mCurrentResults;
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

			std::vector<std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> > mOnEnterEachOverlapFns;
			std::vector<std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> > mDuringEachOverlapFns;
			std::vector<std::function<void(entityx::Entity thisEntity, entityx::Entity overlappingEntity)> > mOnEndEachOverlapFns;

			friend class PhysicsSystem;
		};
	}
}
