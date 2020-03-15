#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "bullet/btBulletDynamicsCommon.h"

namespace sitara {
	namespace ecs {
		namespace physics {
			static ci::vec3 fromBtVector3(const btVector3 &vector) {
				return ci::vec3(vector.getX(), vector.getY(), vector.getZ());
			}

			static ci::mat4 fromBtTransform(const btTransform &transform) {
				// CHECK -- this could be wrong, if you get errors, look here firstr
				float openglMatrix[16];
				transform.getOpenGLMatrix(openglMatrix);

				ci::mat4 modelMatrix = glm::make_mat4(openglMatrix);
				return modelMatrix;
			}

			static ci::quat fromBtQuaternion(const btQuaternion &quat) {
				return ci::quat(quat.getX(), quat.getY(), quat.getZ(), quat.getW());
			}

			static btVector3 toBtVector3(const ci::vec3 &vector) {
				return btVector3(vector.x, vector.y, vector.z);
			}

			static btTransform toBtTransform(const ci::mat4 &matrix) {
				// CHECK -- this could be wrong, if you get errors, look here firstr
				btTransform transform;
				transform.setFromOpenGLMatrix(glm::value_ptr(matrix));
				return transform;
			}

			static btQuaternion toBtQuaternion(const ci::quat &quat) {
				return btQuaternion(quat.x, quat.y, quat.z, quat.w);
			}
		}
	}
}