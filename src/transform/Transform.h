#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"

namespace sitara {
  namespace ecs {
    class Transform {
      public:
        Transform() = delete;

        Transform(entityx::Entity entity, const ci::vec3 &position = ci::vec3(0), const ci::vec3 &scale = ci::vec3(1), const ci::vec3 &anchor = ci::vec3(0), const ci::quat &orientation = ci::quat())
        : mEntity(entity),
          mPosition(position),
          mScale(scale),
          mAnchor(anchor),
          mOrientation(orientation) {
			mParent = invalidHandle();
        }

        ~Transform() {
          removeFromParent();
          for(auto &child : mChildren) {
            child->mParent = invalidHandle();
            child->getEntity().destroy();
          }
        }

		entityx::Entity getEntity() const {
			return mEntity;
		}

        entityx::ComponentHandle<Transform> addChild(entityx::Entity child) {
          entityx::ComponentHandle<Transform> handle;

          if(child.has_component<Transform>()) {
            handle = child.component<Transform>();
          }
          else {
            handle = child.assign<Transform>(child);
          }

          addChild(handle);
          return handle;
        }

        void addChild(entityx::ComponentHandle<Transform> childHandle) {
          if (childHandle.get() != this) {
            childHandle->removeFromParent();
            childHandle->mParent = mEntity.component<Transform>();
            mChildren.push_back(childHandle);
          }
        }

        void removeFromParent() {
          if(mParent) {
            mParent->removeChild(mEntity.component<Transform>());
          }
        }

        void removeChild(entityx::ComponentHandle<Transform> handle) {
          if (handle) {
            handle->mParent = invalidHandle();
            auto begin = std::remove_if(mChildren.begin(), mChildren.end(), [&](const entityx::ComponentHandle<Transform> &entity) {
              return entity == handle;
            });
            mChildren.erase(begin, mChildren.end());
          }
        }

        bool isRoot() const {
          return !mParent;
        }

        bool isLeaf() const {
          return mChildren.empty();
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

		void descend(const std::function<void(const Transform&, Transform&)>& function) {
			for (auto& child : mChildren) {
				function(*this, *child.get());
				child->descend(function);
			}
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
        static entityx::ComponentHandle<Transform> invalidHandle() {
          return entityx::ComponentHandle<Transform>();
        }

        entityx::Entity mEntity;
		entityx::ComponentHandle<Transform> mParent;
        std::vector<entityx::ComponentHandle<Transform>> mChildren;
        ci::mat4 mLocalTransform;
        ci::mat4 mWorldTransform;
    };

	inline entityx::ComponentHandle<Transform> getTransformComponent(entityx::Entity entity) {
		/*
		entityx::ComponentHandle<Transform> handle;
		if (entity.has_component<Transform>()) {
			handle = entity.component<Transform>();
		}
		else {
			handle = entity.assign<Transform>(entity);
		}
		return handle;
		*/
		return entity.has_component<Transform>() ? entity.component<Transform>() : entity.assign<Transform>(entity);
	}

	inline entityx::Entity attachChild(entityx::Entity root, entityx::Entity child) {
		auto root_handle = getTransformComponent(root);
		root_handle->addChild(child);
		return root;
	}

  }
}
