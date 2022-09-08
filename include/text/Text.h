#pragma once

#ifdef USING_SDFTEXT

#include "cinder/gl/SdfText.h"

namespace sitara {
    namespace ecs {
        class Text {
           public:
            Text(const std::string& renderedString,
                        const std::string& styleName,
                        const ci::vec2& baseline = ci::vec2(0),
                        const ci::gl::SdfText::DrawOptions options = ci::gl::SdfText::DrawOptions()) : mUseFitRect(false) {
                mRenderedString = renderedString;
                mStyleName = styleName;
                mBaseline = baseline;
                mFormatOptions = options;
                mStringColor = ci::Color::black();
            }

            Text(const std::string& renderedString,
                 const std::string& styleName,
                 const ci::Rectf& fitRect,
                 const ci::gl::SdfText::DrawOptions options = ci::gl::SdfText::DrawOptions()) : mUseFitRect(true) {
                mRenderedString =renderedString;
                mStyleName = styleName;
                mFitRect = fitRect;
                mFormatOptions = options;
                mStringColor = ci::Color::black();
            }

            void setText(const std::string& renderedString,
                                const ci::vec2& baseline = ci::vec2(0),
                                const ci::gl::SdfText::DrawOptions options = ci::gl::SdfText::DrawOptions()) {
                mRenderedString = renderedString;
                mBaseline = baseline;
                mFormatOptions = options;
                mStringColor = ci::Color::black();
            }

            void setStyleName(const std::string& styleName) { mStyleName = styleName; }

            const std::string& getStyleName() { return mStyleName; }

            const std::string& getString() { return mRenderedString; }

            bool useFitRect() { return mUseFitRect; }

            const ci::Rectf& getFitRect() { return mFitRect; }

            const ci::vec2& getBaseline() { return mBaseline; }

            void setFormatOptions(const ci::gl::SdfText::DrawOptions& options) { mFormatOptions = options; }
            const ci::gl::SdfText::DrawOptions& getFormatOptions() { return mFormatOptions; }

            void setColor(ci::ColorA color) { mStringColor = color; }

            const ci::ColorA& getColor() { return mStringColor; }

           protected:
            std::string mRenderedString;
            std::string mStyleName;
            ci::vec2 mBaseline;
            bool mUseFitRect;
            ci::Rectf mFitRect;
            ci::gl::SdfText::DrawOptions mFormatOptions;
            ci::ColorA mStringColor;

            friend class TextSystem;
        };

        typedef entityx::ComponentHandle<sitara::ecs::Text> TextHandle;
    }  // namespace ecs
}  // namespace sitara

#endif
