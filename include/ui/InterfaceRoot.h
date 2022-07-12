#pragma once

#ifdef USING_SDFTEXT

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/SdfText.h"

#include "entityx/config.h"
#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/quick.h"
#include "entityx/deps/Dependencies.h"

namespace sitara {
namespace ecs {
class InterfaceRoot {
   public:
    InterfaceRoot();
    ~InterfaceRoot();
    void setupSystems();
    virtual void setupFonts() = 0;
    virtual void update(float dt);
    virtual void draw();
    entityx::EntityManager& getEntityManager();
    entityx::SystemManager& getSystemManager();
    void addChild(entityx::Entity& parent, entityx::Entity& child);
    entityx::Entity createTextElement(const ci::vec3& position,
                                      const std::string& fontStyle,
                                      const std::string& text,
                                      const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
    entityx::Entity createImageElement(const ci::vec3& position, ci::gl::Texture2dRef imageTex);
    entityx::Entity createFboElement(const ci::vec3& position, const ci::vec2& size, const ci::gl::Fbo::Format& format = ci::gl::Fbo::Format());
    entityx::Entity createRectangleElement(const ci::vec3& position,
                                           const ci::vec2& size,
                                           const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRectangleElement(const ci::vec3& position,
                                           const ci::geom::Rect& rect,
                                           const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRoundedRectElement(const ci::vec3& position,
                                             const ci::vec2& size,
                                             float radius,
                                             const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRoundedRectElement(const ci::vec3& position,
                                             const ci::geom::RoundedRect& rect,
                                             const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRectangleButton(const ci::vec3& position,
                                          const ci::vec2& size,
                                          const ci::ColorA& color = ci::Color::white());
    entityx::Entity createImageButton(const ci::vec3& position, ci::gl::Texture2dRef imageTex);

   protected:
    entityx::EntityManager mEntities;
    entityx::EventManager mEvents;
    entityx::SystemManager mSystems;
};
}  // namespace ecs
}  // namespace sitara

#endif