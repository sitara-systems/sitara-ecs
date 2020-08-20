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
				mColor = color;
				ci::gl::GlslProgRef shader;

				if (mPrimitiveType < geometry::Primitive::PLANE) {
					// if not a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().lambert().color());
				}
				else {
					// if a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
				}
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			Geometry(const ci::geom::Source& source, ci::gl::GlslProgRef shader, ci::Color color = ci::Color(ci::Color::white())) {
				mPrimitiveType = geometry::checkGeometryType(source);
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			geometry::Primitive getPrimitive() {
				return mPrimitiveType;
			}

			ci::gl::BatchRef getBatch() {
				return mGeometryBatch;
			}

			void setColor(ci::Color color) {
				mColor = color;
			}

			ci::Color& getColor() {
				return mColor;
			}

			void setShader(ci::gl::GlslProgRef shaderProgram) {
				mGeometryBatch->replaceGlslProg(shaderProgram);
			}

			ci::gl::GlslProgRef getShader() {
				return mGeometryBatch->getGlslProg();
			}

			/*
			~Geometry() {
				delete mGeometrySource;
			}
			*/

			void draw() {
				ci::gl::color(mColor);
				mGeometryBatch->draw();
			}

		private:
			ci::Color mColor;
			ci::gl::BatchRef mGeometryBatch;
			sitara::ecs::geometry::Primitive mPrimitiveType;
		};
	}
}