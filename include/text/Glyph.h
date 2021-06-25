#ifdef USING_SDFTEXT

#include "cinder/gl/SdfText.h"

namespace sitara {
	namespace ecs {
		class Glyph {
		public:
			Glyph(const ci::gl::SdfText::Font::Glyph& glyph, const ci::vec2& homePosition) {
				mGlyph = glyph;
				mHomePosition = homePosition;
				mCurrentPosition = ci::vec2(0);
			}

			ci::gl::SdfText::Font::Glyph getGlyph() {
				return mGlyph;
			}
		protected:
			ci::gl::SdfText::Font::Glyph mGlyph;
			ci::vec2 mHomePosition;
			ci::vec2 mCurrentPosition;
		};
	}
}

#endif