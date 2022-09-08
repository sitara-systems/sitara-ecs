#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "entityx/Entity.h"

namespace sitara {
    namespace ecs {
        class Fbo {
       public:
            Fbo(const ci::vec2& size, const ci::gl::Fbo::Format& format) {
                mFbo = ci::gl::Fbo::create(size.x, size.y, format);

                mCamera = ci::CameraPersp(size.x, size.y, 45.0f);
            }

            ~Fbo() {}

            void bindFbo() { mFbo->bindFramebuffer(); }

            void unbindFbo() { mFbo->unbindFramebuffer(); }

            void setDrawFn(std::function<void(Fbo&, entityx::TimeDelta)> fn) { mDrawFn = fn; }

            ci::CameraPersp& getCamera() { return mCamera; }

            void setCamera(const ci::CameraPersp& camera) { mCamera = camera; }

            ci::gl::FboRef getFbo() { return mFbo; }

            ci::gl::Texture2dRef getTexture() { return mFbo->getColorTexture(); }

           protected:
            ci::gl::FboRef mFbo;
            ci::CameraPersp mCamera;
            std::function<void(Fbo&, entityx::TimeDelta)> mDrawFn;

            friend class FboSystem;
        };

        typedef entityx::ComponentHandle<sitara::ecs::Fbo> FboHandle;
    }  // namespace ecs
}  // namespace sitara
