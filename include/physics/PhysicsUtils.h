#pragma once

#include "PxPhysicsAPI.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"

namespace sitara {
	namespace ecs {
		namespace physics {
			static ci::vec2 from(const physx::PxVec2 &vector) {
				return ci::vec2(vector.x, vector.y);
			}

			static ci::vec3 from(const physx::PxVec3 &vector) {
				return ci::vec3(vector.x, vector.y, vector.z);
			}

			static ci::vec4 from(const physx::PxVec4 &vector) {
				return ci::vec4(vector.x, vector.y, vector.z, vector.w);
			}

			static ci::mat3 from(const physx::PxMat33 &matrix) {
				return ci::mat3(
					matrix.column0.x, matrix.column0.y, matrix.column0.z,
					matrix.column1.x, matrix.column1.y, matrix.column1.z,
					matrix.column2.x, matrix.column2.y, matrix.column2.z
				);
			}

			static ci::mat4 from(const physx::PxMat33 &matrix, const physx::PxVec3 &vector) {
				return ci::mat4(
					matrix.column0.x, matrix.column0.y, matrix.column0.z, 0.0f,
					matrix.column1.x, matrix.column1.y, matrix.column1.z, 0.0f,
					matrix.column2.x, matrix.column2.y, matrix.column2.z, 0.0f,
					vector.x, vector.y, vector.z, 1.0f
				);
			}

			static ci::mat4 from(const physx::PxMat44 &matrix) {
				return ci::mat4(
					matrix.column0.x, matrix.column0.y, matrix.column0.z, matrix.column0.w,
					matrix.column1.x, matrix.column1.y, matrix.column1.z, matrix.column1.w,
					matrix.column2.x, matrix.column2.y, matrix.column2.z, matrix.column2.w,
					matrix.column3.x, matrix.column3.y, matrix.column3.z, matrix.column3.w
				);
			}

			static ci::mat4 from(const physx::PxQuat& quat, const physx::PxVec3& vector) {
				return from(physx::PxMat33(quat), vector);
			}

			static ci::mat4 from(const physx::PxTransform &transform) {
				return from(transform.q, transform.p);
			}

			static ci::quat from(const physx::PxQuat &quat) {
				return ci::quat(quat.w, quat.x, quat.y, quat.z);
			}

			static ci::AxisAlignedBox from(const physx::PxBounds3 &bounds) {
				return ci::AxisAlignedBox(from(bounds.minimum), from(bounds.maximum));
			}

			static physx::PxVec2 to(const ci::vec2 &vector) {
				return physx::PxVec2(vector.x, vector.y);
			}

			static physx::PxVec3 to(const ci::vec3 &vector) {
				return physx::PxVec3(vector.x, vector.y, vector.z);
			}

			static physx::PxVec4 to(const ci::vec4 &vector) {
				return physx::PxVec4(vector.x, vector.y, vector.z, vector.w);
			}

			static physx::PxMat33 to(const ci::mat3 &matrix) {
				return physx::PxMat33(
					to(ci::vec3(matrix[0])),
					to(ci::vec3(matrix[1])),
					to(ci::vec3(matrix[2]))
				);
			}

			static physx::PxMat44 to(const ci::mat4 &matrix) {
				return physx::PxMat44(
					to(ci::vec4(matrix[0])),
					to(ci::vec4(matrix[1])),
					to(ci::vec4(matrix[2])),
					to(ci::vec4(matrix[3]))
				);
			}

			static physx::PxQuat to(const ci::quat &quat) {
				return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
			}

			static physx::PxTransform to(const ci::quat &quat, const ci::vec3 &vector) {
				return physx::PxTransform(to(vector), to(quat));
			}

			static physx::PxBounds3 to(const ci::AxisAlignedBox &b) {
				return physx::PxBounds3(to(b.getMin()), to(b.getMax()));
			}
			
			static float clampParticleForce(float magnitude, float maxMagnitude) {
				if (magnitude > maxMagnitude) {
					return maxMagnitude;
				}
				else {
					return magnitude;
				}
			}
		}
	}
}