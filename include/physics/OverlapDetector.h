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
				mQueryFilter() {
				mOverlapShape = new physx::PxSphereGeometry(OverlapDistance);
				mTransform = sitara::ecs::physics::to(ci::quat(), center);
				mIsOverlapping = false;
				mCurrentResults.reserve(4096);
				mPreviousResults.reserve(4096);
			}

			~OverlapDetector() {
				delete mOverlapShape;
			}

			bool isOverlapping() {
				return mIsOverlapping;
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

			physx::PxOverlapHit* getResultsBuffer() {
				return mCurrentResults.data();
			}

			int getResultsBufferSize() {
				return mResultsBufferSize;
			}

			void resizeBuffer(size_t size) {
				mCurrentResults.resize(size);
			}

			physx::PxQueryFilterData& getFilter() {
				return mQueryFilter;
			}

			void swapBuffers() {
				mCurrentResults.swap(mPreviousResults);
				mCurrentResults.clear();
			}

			void applyCollisionLogic() {
				// define how to compare PxOverlapHit objects
				/*
				for (auto& hit : mCurrentResults) {
					if (std::find(mPreviousResults.begin(), mPreviousResults.end(), hit) != mPreviousResults.end()) {
						// in current collision + previous collision, still colliding
						std::cout << "Still Colliding..." << std::endl;
					}
					else {
						// in current collision but NOT previous collision, just started
						std::cout << "Begin Collision!" << std::endl;
					}
				}

				for (auto& hit : mPreviousResults) {
					if (std::find(mCurrentResults.begin(), mCurrentResults.end(), hit) != mCurrentResults.end()) {
						// in previous collision but NOT in current collision, ending collision
						std::cout << "Ending Collision." << std::endl;
					}
				}
				*/
			}

			static const int mResultsBufferSize = 4096;
			bool mIsOverlapping;
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
