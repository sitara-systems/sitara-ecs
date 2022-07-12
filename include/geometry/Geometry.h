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
            Geometry(const ci::gl::BatchRef batch) : mUseAssimp(false), mUseTexture(false), mTexture(nullptr) {
				mGeometryBatch = batch;
				// can't get the source geometry from the batch, so need to declare this an unknown primitive
				mPrimitiveType = geometry::Primitive::UNKNOWN;
			}

			Geometry(const ci::geom::Source& source, ci::Color color = ci::Color::white())
                            : mUseAssimp(false), mUseTexture(false), mTexture(nullptr) {
				mPrimitiveType = geometry::checkGeometryType(source);
				mColor = color;

                ci::gl::ShaderDef shaderConfig = ci::gl::ShaderDef().color();

				if (mPrimitiveType <= geometry::Primitive::RING) {
					// if not a wireframe
                    shaderConfig.lambert();
 				}

				ci::gl::GlslProgRef shader = ci::gl::getStockShader(shaderConfig);
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			Geometry(const ci::geom::Source& source, ci::gl::Texture2dRef texture)
                : mUseAssimp(false), mUseTexture(true) {
                mPrimitiveType = geometry::checkGeometryType(source);
                mColor = ci::Color::white();
                mTexture = texture;

                ci::gl::ShaderDef shaderConfig = ci::gl::ShaderDef().color().lambert().texture();
                ci::gl::GlslProgRef shader = ci::gl::getStockShader(shaderConfig);
                mGeometryBatch = ci::gl::Batch::create(source, shader);
            }

			Geometry(const ci::geom::Source& source,
                                 ci::gl::GlslProgRef shader,
                                 ci::Color color = ci::Color::white())
                : mUseAssimp(false), mUseTexture(false), mTexture(nullptr) {
				mPrimitiveType = geometry::checkGeometryType(source);
				mColor = color;
				mGeometryBatch = ci::gl::Batch::create(source, shader);
			}

			#ifdef USING_ASSIMP
                        Geometry(const std::filesystem::path& filename)
                            : mUseAssimp(true), mUseTexture(false), mTexture(nullptr) {
				mPrimitiveType = geometry::Primitive::UNKNOWN;
				std::filesystem::path modelPath = ci::app::getAssetPath(filename);
				if (!modelPath.empty()) {
					mModelLoader = sitara::assimp::AssimpLoader(modelPath);
				}
				else {
					CI_LOG_W("Could not find 3d model file " << modelPath << "; please check for this file.");
				}
			}

			Geometry(const sitara::assimp::AssimpLoader& loader)
                            : mUseAssimp(true), mUseTexture(false), mTexture(nullptr) {
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

			void setColor(ci::ColorA color) {
				mColor = color;
			}

			ci::ColorA& getColor() {
				return mColor;
			}

			void setShader(ci::gl::GlslProgRef shaderProgram) {
				mGeometryBatch->replaceGlslProg(shaderProgram);
			}

			ci::gl::GlslProgRef getShader() {
				return mGeometryBatch->getGlslProg();
			}

			void setTexture(ci::gl::Texture2dRef texture) {
                mUseTexture = true;
                mTexture = texture;
			}

			ci::gl::Texture2dRef getTexture() {
				return mTexture;
			}

			void draw() {
				#ifdef USING_ASSIMP
				if (mUseAssimp) {
					mModelLoader.draw();
				}
				else {
					ci::gl::ScopedColor scopedColor(mColor);
					mGeometryBatch->draw();
				}
				#else
                ci::gl::ScopedColor scopedColor(mColor);
                if (mUseTexture) {
                    ci::gl::ScopedTextureBind scopedTexture(mTexture, 0);
                    mGeometryBatch->draw();
                } else {
                    mGeometryBatch->draw();                
				}
				#endif
			}

		private:
			ci::ColorA mColor;
            bool mUseTexture;
            ci::gl::Texture2dRef mTexture;
			ci::gl::BatchRef mGeometryBatch;
			sitara::ecs::geometry::Primitive mPrimitiveType;
			bool mUseAssimp;
			#ifdef USING_ASSIMP
			sitara::assimp::AssimpLoader mModelLoader;
			#endif
		};
	}
}