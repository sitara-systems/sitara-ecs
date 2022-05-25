#pragma once

#ifdef USING_SDFTEXT

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "text/TextSystem.h"

using namespace sitara::ecs;

TextSystem::TextSystem() {
}

TextSystem::~TextSystem() {
}

void TextSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
}

void TextSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
}

void TextSystem::registerStyle(const std::string& styleName, const std::filesystem::path& path, float fontSize, const ci::ColorA& color) {
	std::string cachedPath = text::getCachedFilePath(path);
	auto fontInstance = ci::gl::SdfText::create(cachedPath, ci::gl::SdfText::Font(ci::app::loadAsset(path.string()), fontSize));
	mFontInstances.insert(std::pair<std::string, ci::gl::SdfTextRef>(styleName, fontInstance));
    mStyleColors.insert(std::pair<std::string, ci::ColorA>(styleName, color));
}

entityx::ComponentHandle<sitara::ecs::GlyphString> TextSystem::addGlyphStringComponent(entityx::Entity& entity,
                                                      const std::string& styleName,
                                                      const std::string& string,
                                                      const ci::vec2& baseline) {
    std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> titleString =
        getGlyphPlacements(styleName, string);
    auto styleColor = mStyleColors.find(styleName);
    if (styleColor == mStyleColors.end()) {
        std::cout << "Could not find style with name " << styleName << "; cannot draw Glyphs!"
                  << std::endl;
        return entityx::ComponentHandle<sitara::ecs::GlyphString>();
    }
    auto component = entity.assign<sitara::ecs::GlyphString>(titleString, styleName, string, baseline);
    component->setColor((*styleColor).second);
    return component;
}

std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> TextSystem::getGlyphPlacements(const std::string& fontName, const std::string& str, const ci::gl::SdfText::DrawOptions& options) {
	ci::gl::SdfTextRef fontRenderer = mFontInstances[fontName];
	return fontRenderer->getGlyphPlacements(str, options);
}

std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> TextSystem::getGlyphPlacementsWrapped(const std::string& fontName, const std::string& str, const ci::Rectf& fitRect, const ci::gl::SdfText::DrawOptions& options) {
	ci::gl::SdfTextRef fontRenderer = mFontInstances[fontName];
	ci::gl::SdfText::Font::GlyphMeasuresList measures = fontRenderer->getGlyphPlacementsWrapped(str, fitRect, options);
	for (auto& glyph : measures) {
		glyph.second += fitRect.getUpperLeft();
	}
	return measures;
}

void TextSystem::drawGlyphs(const std::string& fontName, std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> glyphPlacements, const ci::vec2& baseline, const ci::gl::SdfText::DrawOptions& options) {
	ci::gl::SdfTextRef fontRenderer = mFontInstances[fontName];
	if (!fontRenderer) {
		std::cout << "Could not find style with name " << fontName << "; cannot draw Glyphs!" << std::endl;
		return;
	}
	fontRenderer->drawGlyphs(glyphPlacements, baseline, options);
}

void TextSystem::drawGlyphString(entityx::ComponentHandle<sitara::ecs::GlyphString> glyphString) {
    ci::gl::SdfTextRef fontRenderer = mFontInstances[glyphString->getStyleName()];
    if (!fontRenderer) {
        std::cout << "Could not find style with name " << glyphString->getStyleName() << "; cannot draw Glyphs!" << std::endl;
        return;
    }
    ci::gl::ScopedColor color(glyphString->getColor());
    fontRenderer->drawGlyphs(glyphString->getGlyphString(), glyphString->getBaseline(), glyphString->getOptions());
}

#endif