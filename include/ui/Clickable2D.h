#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"
#include "geometry/GeometryUtils.h"

namespace sitara {
	namespace ecs {
		class Clickable2D {
		public:
            Clickable2D()
                : mOnDownFn(nullptr),
                    mOnDragFn(nullptr),
                    mOnMoveFn(nullptr),
                    mOnUpFn(nullptr),
                    mOnWheelFn(nullptr){}

			Clickable2D(const ci::geom::Source& source)
                        : mOnDownFn(nullptr),
                          mOnDragFn(nullptr),
                          mOnMoveFn(nullptr),
                          mOnUpFn(nullptr),
                          mOnWheelFn(nullptr) {
                // geom::Rect/RoundedRect/Circle are one-way mesh-generation
                // sources with no accessors back to their input parameters,
                // so recover the 2D bounds via the mesh's own bounding box.
                ci::AxisAlignedBox box = sitara::ecs::geometry::getBoundingBox(source);
                mBoundingBox = ci::Rectf(ci::vec2(box.getMin()), ci::vec2(box.getMax()));
            }

            ci::Rectf getBoundingBox() {
                return mBoundingBox;
            }

            void setOnDownFn(std::function<void(entityx::Entity&)> fn) { mOnDownFn = fn; }
            void setOnDragFn(std::function<void(entityx::Entity&)> fn) { mOnDragFn = fn; }
            void setOnMoveFn(std::function<void(entityx::Entity&)> fn) { mOnMoveFn = fn; }
            void setOnUpFn(std::function<void(entityx::Entity&)> fn) { mOnUpFn = fn; }
            void setOnWheelFn(std::function<void(entityx::Entity&)> fn) { mOnWheelFn = fn; }

		private:
            ci::Rectf mBoundingBox;
            std::function<void(entityx::Entity&)> mOnDownFn;
            std::function<void(entityx::Entity&)> mOnDragFn;
            std::function<void(entityx::Entity&)> mOnMoveFn;
            std::function<void(entityx::Entity&)> mOnUpFn;
            std::function<void(entityx::Entity&)> mOnWheelFn;

            friend class MouseSystem;
        };

        typedef entityx::ComponentHandle<Clickable2D> Clickable2DHandle;

	}
}
