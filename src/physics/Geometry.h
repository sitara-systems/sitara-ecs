#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"

namespace sitara {
	namespace ecs {
		enum Primitive { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE, RECT, ROUNDEDRECT, CIRCLE, RING, PRIMITIVE_COUNT };
		enum Quality { LOW, DEFAULT, HIGH };
		enum ViewMode { SHADED, WIREFRAME };
		enum TexturingMode { NONE, PROCEDURAL, SAMPLER };

		class Geometry {
		public:
			Geometry(const ci::geom::Source& source, ci::Color color) : mColor(color) {
				auto glColor = ci::geom::Constant(ci::geom::COLOR, mColor);
				auto lambert = ci::gl::ShaderDef().lambert().color();
				auto shader = ci::gl::getStockShader(lambert);
				mGeometry = ci::gl::Batch::create(source >> glColor, shader);
			}

			/*
			~Geometry() {
				delete mGeometrySource;
			}
			*/

			void draw() {
				mGeometry->draw();
			}

		private:
			ci::Color mColor;
			ci::gl::BatchRef mGeometry;
		};
	}
}