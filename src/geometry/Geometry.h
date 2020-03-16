#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"

namespace sitara {
	namespace ecs {
		class Geometry {
		public:
			Geometry(ci::geom::Source& source, ci::Color color) : mColor(color) {
				auto glColor = ci::geom::Constant(ci::geom::COLOR, mColor);
				auto lambert = ci::gl::ShaderDef().lambert().color();
				auto shader = ci::gl::getStockShader(lambert);
				mGeometry = ci::gl::Batch::create(source >> glColor, shader);
			}

			void draw() {
				mGeometry->draw();
			}

		private:
			ci::Color mColor;
			ci::gl::BatchRef mGeometry;
		};
	}
}