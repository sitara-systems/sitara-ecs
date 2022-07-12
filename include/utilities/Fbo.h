#pragma once

#include "cinder/gl/gl.h"

namespace sitara {
    namespace ecs {
        class Fbo {
       public:
            Fbo(const ci::vec2& size, const ci::gl::Fbo::Format& format) {
                mFbo = ci::gl::Fbo::create(size.x, size.y, format);

                mCamera = ci::CameraPersp(size.x, size.y, 45.0f);
            }

            ~Fbo() {

            }

            ci::gl::FboRef getFbo() {
                return mFbo;
            }

            void bindFbo() {
                mFbo->bindFramebuffer();
            }

            void unbindFbo() {
                mFbo->unbindFramebuffer();
            }

            ci::gl::Texture2dRef getTexture() {
                return mFbo->getColorTexture();
            }

            ci::CameraPersp& getCamera() {
                return mCamera;
            }

            void setCamera(const ci::CameraPersp& camera) {
                mCamera = camera;
            }
        protected:
            ci::gl::FboRef mFbo;
            ci::CameraPersp mCamera;
            float mCameraDistance;
            ci::vec3 mEye;
            ci::vec3 mCenter;
            ci::vec3 mUp;
        };
    }  // namespace ecs
}  // namespace sitara
