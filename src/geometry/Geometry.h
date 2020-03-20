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
				mPrimitiveType = geometry::checkGeometryType(source);
				auto glColor = ci::geom::Constant(ci::geom::COLOR, color);
				ci::gl::GlslProgRef shader;

				if (mPrimitiveType < geometry::Primitive::PLANE) {
					// if not a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().lambert().color());
				}
				else {
					// if a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
				}
				mGeometryBatch = ci::gl::Batch::create(source >> glColor, shader);
			}

			Geometry(const ci::geom::Source& source, ci::gl::GlslProgRef shader, ci::Color color = ci::Color(ci::Color::white())) {
				mPrimitiveType = geometry::checkGeometryType(source);
				ci::geom::Constant glColor = ci::geom::Constant(ci::geom::COLOR, color);
				mGeometryBatch = ci::gl::Batch::create(source, shader);
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