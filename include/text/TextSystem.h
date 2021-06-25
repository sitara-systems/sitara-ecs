#pragma once

#ifdef USING_SDFTEXT

#include "entityx/System.h"
#include "cinder/gl/SdfText.h"

namespace sitara {
    namespace ecs {
        class TextSystem : public entityx::System<TextSystem>, public entityx::Receiver<TextSystem> {
        public:
            TextSystem();
            ~TextSystem();
            void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
            void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
            void registerFont(const std::string& name, const std::filesystem::path& path, float fontSize = 32.0f);
            std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> getGlyphPlacements(const std::string& fontName, const std::string& str, const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
            void drawGlyphs(const std::string& fontName, std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> glyphPlacements, const ci::vec2& baseline, const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
        protected:
            std::map<std::string, ci::gl::SdfTextRef> mFontInstances;
        };
    }
}

#endif