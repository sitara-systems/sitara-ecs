#pragma once 

#ifdef USING_SDFTEXT

#include "cinder/gl/SdfText.h"

namespace sitara {
	namespace ecs {
		class GlyphString {
		public:
            GlyphString(const std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>>& renderedString,
                        const std::string& styleName,
                        const std::string& textString,
                        const ci::vec2& baseline = ci::vec2(0),
                        const ci::gl::SdfText::DrawOptions options = ci::gl::SdfText::DrawOptions()) {
                mGlyphString = renderedString;
				mStyleName = styleName;
                mString = textString;
                mBaseline = baseline;
                mFormatOptions = options;
                mTextColor = ci::Color::black();                
            }

            void setGlyphString(
                            const std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>>& string,
                            const std::string& textString,
                            const ci::vec2& baseline = ci::vec2(0),
                            const ci::gl::SdfText::DrawOptions options = ci::gl::SdfText::DrawOptions()) {
                mGlyphString = string;
                mString = textString;
                mBaseline = baseline;
                mFormatOptions = options;
                mTextColor = ci::Color::black();
            }

			const std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>>& getGlyphString() {
				return mGlyphString;
			}

            void setStyleName(const std::string& styleName) {
                mStyleName = styleName;
            }

            const std::string& getStyleName() {
                return mStyleName;
            }

			const std::string& getString() {
				return mString;
			}

            const ci::vec2& getBaseline() {
                return mBaseline;
            }

            const ci::gl::SdfText::DrawOptions& getOptions() {
                return mFormatOptions;
            }

            void setColor(ci::ColorA color) {
                mTextColor = color;
            }

            const ci::ColorA& getColor() {
return mTextColor;
            }

		protected:
			std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> mGlyphString;
			std::string mStyleName;
            std::string mString;
            ci::vec2 mBaseline;
            ci::gl::SdfText::DrawOptions mFormatOptions;
            ci::ColorA mTextColor;
		};
	}
}

#endif
