#pragma once

#ifdef USING_SDFTEXT

#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include <jsoncpp/json.h>
#include "cinder/Log.h"
#include "text/TextSystem.h"

using namespace sitara::ecs;

TextSystem::TextSystem() {
}

TextSystem::~TextSystem() {
}

void TextSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
}

void TextSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {}

void TextSystem::loadFontsFromFile(const std::filesystem::path& path) {
    Json::Reader jsonReader;
    Json::Value root;

    std::filesystem::path assetPath = ci::app::getAssetPath(path);
    if (assetPath.empty()) {
        CI_LOG_F("Could not find file " << path << "; please check the file and try again.");
        std::exit(0);
    }

    bool result = jsonReader.parse(ci::loadString(ci::app::loadAsset(path)), root);
    if (!result) {
        CI_LOG_F("Error reading json file " << path << "; please check for valid JSON.");
        std::exit(0);
    }

    if (root["fonts"].isArray()) {
        for (Json::Value& fontObj : root["fonts"]) {
            registerStyle(fontObj["name"].asString(), fontObj["font_path"].asString(), fontObj["font_size"].asInt());
        }    
    } else {
        CI_LOG_F("Error reading json file " << path << "; could not an array called 'fonts'.  Please read the README.md for TextSystem and format the file again.");
        std::exit(0);    
    }
}

void TextSystem::registerStyle(const std::string& styleName, const std::filesystem::path& path, float fontSize, const ci::ColorA& color) {
	std::string cachedPath = text::getCachedFilePath(path);

    if (!std::filesystem::exists(cachedPath)) {
        CI_LOG_I(
            "Could not find sdf file for fonts, need to build files.  This may "
            "take a few minutes.");
    }

	auto fontInstance = ci::gl::SdfText::create(cachedPath, ci::gl::SdfText::Font(ci::app::loadAsset(path.string()), fontSize));
	mFontInstances.insert(std::pair<std::string, ci::gl::SdfTextRef>(styleName, fontInstance));
    //mStyleColors.insert(std::pair<std::string, ci::ColorA>(styleName, color));
}

void TextSystem::setGlyphColor(entityx::Entity& entity, const ci::ColorA& color) {
    entityx::ComponentHandle<Glyph> glyph;
    sitara::ecs::TextHandle text;

    entity.unpack(glyph, text);

    if (glyph.valid()) {
        // TO DO: add color parameters to individual glyphs for drawing
    }
    if (text.valid()) {
        text->setColor(color);
    }
}

sitara::ecs::TextHandle TextSystem::addTextComponent(entityx::Entity& entity,
    const std::string& styleName,
    const std::string& string,
    const ci::vec2& baseline,
    const ci::gl::SdfText::DrawOptions& options) {
    auto component = entity.assign<sitara::ecs::Text>(string, styleName, baseline, options);
    //component->setColor((*styleColor).second);
    return component;
}

sitara::ecs::TextHandle TextSystem::addTextComponent(
    entityx::Entity& entity,
    const std::string& styleName,
    const std::string& string,
    const ci::Rectf& fitRect,
    const ci::gl::SdfText::DrawOptions& options) {
    auto component = entity.assign<sitara::ecs::Text>(string, styleName, fitRect, options);
    if (options.getResizeFitRect()) {
        // update the desired fitRect to the ACTUAL fitRect
        component->mFitRect = measureTextFitRect(component);    
    }
    // component->setColor((*styleColor).second);
    return component;
}

std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> TextSystem::getGlyphPlacements(const std::string& fontName, const std::string& str, const ci::gl::SdfText::DrawOptions& options) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(fontName);
    return fontRenderer->getGlyphPlacements(str, options);
}

std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> TextSystem::getGlyphPlacementsWrapped(const std::string& fontName, const std::string& str, const ci::Rectf& fitRect, const ci::gl::SdfText::DrawOptions& options) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(fontName);
    ci::gl::SdfText::Font::GlyphMeasuresList measures = fontRenderer->getGlyphPlacementsWrapped(str, fitRect, options);
	for (auto& glyph : measures) {
		glyph.second += fitRect.getUpperLeft();
	}
	return measures;
}

float TextSystem::measureLineHeight(sitara::ecs::TextHandle textHandle) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
    return fontRenderer->measureLineHeight(textHandle->getFormatOptions());
}

ci::Rectf TextSystem::measureTextFitRect(sitara::ecs::TextHandle textHandle) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
    ci::Rectf fitRect = textHandle->getFitRect();
    ci::gl::SdfTextBox tbox = ci::gl::SdfTextBox(fontRenderer.get());

    tbox.text(textHandle->getString())
        .size((int)fitRect.getWidth(), ci::gl::SdfTextBox::GROW)
        .ligate(textHandle->getFormatOptions().getLigate())
        .tracking(textHandle->getFormatOptions().getTracking());
    ci::gl::SdfText::Font::GlyphMeasuresList glyphMeasures = tbox.measureGlyphs(textHandle->getFormatOptions());
    return ci::Rectf(fitRect.getUpperLeft(), fitRect.getUpperLeft() + ci::vec2(tbox.getSize()));
}

ci::Rectf TextSystem::measureTextBounds(sitara::ecs::TextHandle textHandle) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
    return fontRenderer->measureStringBounds(textHandle->getString(), textHandle->getFormatOptions());
}

void TextSystem::updateLeadingFromLineHeight(sitara::ecs::TextHandle textHandle,
                                                          float lineHeight) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
    ci::gl::SdfText::DrawOptions options = textHandle->getFormatOptions();
    fontRenderer->updateLeadingFromLineHeight(options, lineHeight);
    textHandle->setFormatOptions(options);
    textHandle->mFitRect = measureTextFitRect(textHandle);
}

void TextSystem::drawGlyphs(const std::string& fontName, std::vector<std::pair<ci::gl::SdfText::Font::Glyph, ci::vec2>> glyphPlacements, const ci::vec2& baseline, const ci::gl::SdfText::DrawOptions& options) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(fontName);
	fontRenderer->drawGlyphs(glyphPlacements, baseline, options);
}

void TextSystem::drawText(sitara::ecs::TextHandle textHandle, ci::ColorA tint) {
    ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
    ci::gl::ScopedColor color(tint * textHandle->getColor());
    if (textHandle->useFitRect()) {
        fontRenderer->drawString(textHandle->getString(), textHandle->getFitRect(), ci::vec2(0), textHandle->getFormatOptions());
    } else {
        fontRenderer->drawString(textHandle->getString(), textHandle->getBaseline(), textHandle->getFormatOptions());    
    }
}

void TextSystem::drawTextDebug(sitara::ecs::TextHandle textHandle) {
    ci::gl::ScopedColor color(textHandle->getColor());
    if (textHandle->useFitRect()) {
        ci::gl::drawStrokedRect(textHandle->getFitRect());
    } else {
        ci::gl::SdfTextRef fontRenderer = getSdfFont(textHandle->getStyleName());
        ci::vec2 size = fontRenderer->measureString(textHandle->getString(), textHandle->getFormatOptions());
        ci::gl::drawLine(textHandle->getBaseline(), textHandle->getBaseline() + ci::vec2(size));
    }
}

ci::gl::SdfTextRef TextSystem::getSdfFont(const std::string& fontName) {
    ci::gl::SdfTextRef fontRenderer = mFontInstances[fontName];
    if (!fontRenderer) {
        std::cout << "Could not find style with name " << fontName << "; cannot draw Glyph component!" << std::endl;
        return nullptr;
    }
    return fontRenderer;
}

#endif