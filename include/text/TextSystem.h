#pragma once

#ifdef USING_SDFTEXT

#include "entityx/System.h"
#include "cinder/gl/SdfText.h"
#include "cinder/Log.h"
#include "text/Glyph.h"
#include "text/GlyphString.h"

namespace sitara {
    namespace ecs {
        namespace text {
            static std::string getCachedFilePath(const std::filesystem::path& filepath) {
                std::string sdft_filename = filepath.stem().string();
                sdft_filename += ".sdft";
                std::string cachedPath = ci::app::getAssetPath("").string() + "/sdft/" + sdft_filename;
                return cachedPath;
            }
        }

        class TextSystem : public entityx::System<TextSystem>, public entityx::Receiver<TextSystem> {
        public:
            TextSystem();
            ~TextSystem();
            void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
            void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
            void registerStyle(const std::string& name, const std::filesystem::path& path, float fontSize = 32.0f, const ci::ColorA& color = ci::Color::black());
            entityx::ComponentHandle<sitara::ecs::GlyphString> addGlyphStringComponent(entityx::Entity& entity, const std::string& styleName, const std::string& string, const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions(), const ci::vec2& baseline = ci::vec2(0));
            std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> getGlyphPlacements(const std::string& fontName, const std::string& str, const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
            std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> getGlyphPlacementsWrapped(const std::string& fontName, const std::string& str, const ci::Rectf& fitRect, const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
            void drawGlyphs(const std::string& fontName, std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> glyphPlacements, const ci::vec2& baseline = ci::vec2(0), const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
            void drawGlyphString(entityx::ComponentHandle<sitara::ecs::GlyphString> glyphString);
        protected:
            std::map<std::string, ci::gl::SdfTextRef> mFontInstances;
            std::map<std::string, ci::ColorA> mStyleColors;
        };
    }
}

#endif