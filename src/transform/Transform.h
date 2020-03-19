#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"

namespace sitara {
	namespace ecs {
		class Transform {
		public:
			//Transform() : mPosition(ci::vec3(0)), mScale(ci::vec3(1)), mAnchor(ci::vec3(0)), mOrientation(ci::quat()) {};

			Transform(const ci::vec3 &position = ci::vec3(0), const ci::vec3 &scale = ci::vec3(1), const ci::vec3 &anchor = ci::vec3(0), const ci::quat &orientation = ci::quat())
				: mPosition(position),
				mScale(scale),
				mAnchor(anchor),
				mOrientation(orientation) {
				mParent = invalidHandle();
			}

			~Transform() {
				for (auto &child : mChildren) {
					child->mParent = invalidHandle();
					// TO DO -- should entity get destroyed when we destroy the Transform Component?
					//child->getEntity().destroy();
				}
			}

			bool isRoot() const {
				return !mParent;
			}

			bool isLeaf() const {
				return mChildren.empty();
			}

			void setParent(entityx::ComponentHandle<Transform> parent) {
				mParent = parent;
			}

			entityx::ComponentHandle<Transform> getParent() const {
				return mParent;
			}

			const std::vector<entityx::ComponentHandle<Transform>>& getChildren() {
				return mChildren;
			}

			size_t getNumberOfChildren() const {
				return mChildren.size();
			}

			const ci::mat4& getWorldTransform() const {
				return mWorldTransform;
			}

			const ci::mat4& getLocalTransform() const {
				return mLocalTransform;
			}

			void updateWorldTransform(const ci::mat4 &parentTransform) {
				mLocalTransform = calcLocalTransform();
				mWorldTransform = parentTransform * mLocalTransform;
			}

			ci::mat4 calcLocalTransform() const {
				return glm::translate(mPosition + mAnchor) * glm::toMat4(mOrientation) * glm::scale(mScale) * glm::translate(-mAnchor / mScale);
			}

			ci::vec3 mPosition;
			ci::vec3 mScale;
			ci::vec3 mAnchor; // relative center of orientation and scaling.
			ci::quat mOrientation;

		private:
			void addChild(entityx::ComponentHandle<Transform> childHandle) {
				mChildren.push_back(childHandle);
			}

			void removeChild(entityx::ComponentHandle<Transform> childHandle) {
				if (childHandle) {
					childHandle->setParent(invalidHandle());
					auto begin = std::remove_if(mChildren.begin(), mChildren.end(), [&](const entityx::ComponentHandle<Transform> &entity) {
						return entity == childHandle;
					});
					mChildren.erase(begin, mChildren.end());
				}
			}

			static entityx::ComponentHandle<Transform> invalidHandle() {
				return entityx::ComponentHandle<Transform>();
			}

			entityx::ComponentHandle<Transform> mParent;
			std::vector<entityx::ComponentHandle<Transform>> mChildren;
			ci::mat4 mLocalTransform;
			ci::mat4 mWorldTransform;

			friend class TransformSystem;
		};
	}
}
