#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"
#include "geometry/GeometryUtils.h"
#include <typeinfo>

namespace sitara {
	namespace ecs {
		class Geometry {
		public:
			Geometry(const ci::gl::BatchRef batch) {
				mGeometryBatch = batch;
				// can't get the source geometry from the batch, so need to declare this an unknown primitive
				mPrimitiveType = geometry::Primitive::UNKNOWN;
			}

			Geometry(const ci::geom::Source& source, ci::Color color) {
				auto glColor = ci::geom::Constant(ci::geom::COLOR, color);
				auto lambert = ci::gl::ShaderDef().lambert().color();
				auto shader = ci::gl::getStockShader(lambert);
				mGeometryBatch = ci::gl::Batch::create(source >> glColor, shader);
				mPrimitiveType = geometry::checkGeometryType(source);
			}

			Geometry(geometry::Primitive primitive, geometry::Quality quality, geometry::ViewMode viewMode, geometry::TexturingMode texMode) {
				/*
				auto glColor = ci::geom::Constant(ci::geom::COLOR, mColor);
				auto lambert = ci::gl::ShaderDef().lambert().color();
				auto shader = ci::gl::getStockShader(lambert);
				mGeometryBatch = ci::gl::Batch::create(source >> glColor, shader);
				mPrimitiveType = geometry::checkGeometryType(source);
				*/
			}

			geometry::Primitive getPrimitive() {
				return mPrimitiveType;
			}
			/*
			~Geometry() {
				delete mGeometrySource;
			}
			*/

			void draw() {
				mGeometryBatch->draw();
			}

		private:
			ci::gl::BatchRef mGeometryBatch;
			sitara::ecs::geometry::Primitive mPrimitiveType;
		};
	}
}