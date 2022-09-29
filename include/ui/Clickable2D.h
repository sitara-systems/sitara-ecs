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
                auto primitive = sitara::ecs::geometry::checkGeometryType(source);
                if (primitive == sitara::ecs::geometry::Primitive::RECT) {
                    const ci::geom::Rect rect = dynamic_cast<const ci::geom::Rect&>(source);
                    mBoundingBox = rect.getRect();
                }
                else if (primitive == sitara::ecs::geometry::Primitive::ROUNDED_RECT) {
                    const ci::geom::RoundedRect rect = dynamic_cast<const ci::geom::RoundedRect&>(source);
                    mBoundingBox = rect.getRect();
                }
                else if (primitive == sitara::ecs::geometry::Primitive::CIRCLE) {
                    const ci::geom::Circle circle = dynamic_cast<const ci::geom::Circle&>(source);
                    ci::vec2 center = circle.getCenter();
                    float radius = circle.getRadius();
                    ci::vec2 topLeft = center - ci::vec2(radius);
                    ci::vec2 bottomRight = center + ci::vec2(radius);
                    mBoundingBox = ci::Rectf(topLeft, bottomRight);
                }
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
