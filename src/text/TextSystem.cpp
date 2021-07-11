#pragma once

#ifdef USING_SDFTEXT

#include "cinder/app/App.h"
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

void TextSystem::registerFont(const std::string& name, const std::filesystem::path& path, float fontSize) {
	std::string cachedPath = text::getCachedFilePath(path);
	auto fontInstance = ci::gl::SdfText::create(cachedPath, ci::gl::SdfText::Font(ci::app::loadAsset(path.string()), fontSize));
	mFontInstances.insert(std::pair<std::string, ci::gl::SdfTextRef>(name, fontInstance));
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
		std::cout << "Could not find font with name " << fontName << "; cannot draw Glyphs!" << std::endl;
		return;
	}
	fontRenderer->drawGlyphs(glyphPlacements, baseline, options);
}

#endif