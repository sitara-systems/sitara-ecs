#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"
#include "geometry/GeometryUtils.h"

namespace sitara {
	namespace ecs {
		class Clickable2D {
		public:
            Clickable2D() = default;

			Clickable2D(const ci::geom::Source& source) {
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

		private:
            ci::Rectf mBoundingBox;
		};
	}
}
