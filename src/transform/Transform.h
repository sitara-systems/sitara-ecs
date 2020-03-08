#pragma once

#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"

namespace sitara {
  namespace ecs {
    namespace components {
      struct Transform {
        TransformComponent() = default;
      	~TransformComponent() = default;
        const ci::mat4& getWorldTransform() const { return mWorldTransform; }
        const ci::mat4& getLocalTransform() const { return mLocalTransform; }

      private:
        void composeTransform(const ci::mat4 &transform) {
          _local_transform = calcLocalTransform();
          _world_transform = transform * _local_transform;
        }

        ci::mat4 calcLocalTransform() { return glm::translate(position + pivot) * glm::toMat4(orientation) * glm::scale(scale) * glm::translate(- pivot / scale); }

        ci::vec3 mPosition;
      	ci::vec3 mScale;
      	ci::vec3 mAnchor;
      	ci::quat mRotation;
      	ci::mat4 mTransform;

      	//world transform, propgated through the scenegraph
      	ci::vec3 mWorldPosition;
      	ci::vec3 mWorldScale;
      	ci::vec3 mWorldAnchor;
      	ci::quat mWorldRotation;
      	ci::mat4 mWorldTransform;
      	bool mWorldTransformDirty;

      	//local transform, not propogated through the scene graph
      	ci::vec3 mLocalPosition;
      	ci::vec3 mLocalScale;
      	ci::vec3 mLocalAnchor;
      	ci::quat mLocalRotation;
      	ci::mat4 mLocalTransform;
      	bool mLocalTransformDirty;

      	uint32_t mLevel;
      	Handle mParent;
      	std::list<Handle> mChildren;
      }
    }
  }
}
