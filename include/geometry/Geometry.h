#pragma once

#include "entityx/Entity.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "geometry/GeometryUtils.h"
#include <typeinfo>

#ifdef USING_ASSIMP
#include "AssimpLoader.h"
#endif

namespace sitara {
	namespace ecs {
		class Geometry {
		public:
			Geometry(const ci::gl::BatchRef batch) : mUseAssimp(false) {
				mGeometryBatch = batch;
				// can't get the source geometry from the batch, so need to declare this an unknown primitive
				mPrimitiveType = geometry::Primitive::UNKNOWN;
			}

			Geometry(const ci::geom::Source& source, ci::Color color) : mUseAssimp(false) {
				mPrimitiveType = geometry::checkGeometryType(source);
				mColor = color;
				ci::gl::GlslProgRef shader;

				if (mPrimitiveType <= geometry::Primitive::PLANE) {
					// if not a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().lambert().color());
				}
				else {
					// if a wireframe
					shader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
				}
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			Geometry(const ci::geom::Source& source, ci::gl::GlslProgRef shader, ci::Color color = ci::Color(ci::Color::white())) : mUseAssimp(false) {
				mPrimitiveType = geometry::checkGeometryType(source);
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			#ifdef USING_ASSIMP
			Geometry(const std::filesystem::path& filename) : mUseAssimp(true) {
				mPrimitiveType = geometry::Primitive::UNKNOWN;
				std::filesystem::path modelPath = ci::app::getAssetPath(filename);
				if (!modelPath.empty()) {
					mModelLoader = mndl::assimp::AssimpLoader(modelPath);
				}
				else {
					CI_LOG_W("Could not find 3d model file " << modelPath << "; please check for this file.");
				}
			}

			Geometry(const mndl::assimp::AssimpLoader& loader) : mUseAssimp(true) {
				mPrimitiveType = geometry::Primitive::UNKNOWN;
				mModelLoader = loader;
			}
			#endif

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
				#ifdef USING_ASSIMP
				if (mUseAssimp) {
					mModelLoader.draw();
				}
				else {
					ci::gl::ScopedColor scopedColor(mColor);
					mGeometryBatch->draw();
				}
				#elif
				ci::gl::ScopedColor scopedColor(mColor);
				mGeometryBatch->draw();
				#endif
			}

		private:
			ci::Color mColor;
			ci::gl::BatchRef mGeometryBatch;
			sitara::ecs::geometry::Primitive mPrimitiveType;
			bool mUseAssimp;
			#ifdef USING_ASSIMP
			mndl::assimp::AssimpLoader mModelLoader;
			#endif
		};
	}
}