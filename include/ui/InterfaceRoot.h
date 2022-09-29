#pragma once

#ifdef USING_SDFTEXT

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/SdfText.h"
#include "cinder/Timeline.h"

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
    enum NodeState { HIDE, SHOW };
    enum ButtonState { INACTIVE, ACTIVE };
    InterfaceRoot();
    ~InterfaceRoot();
    void setupSystems();
    virtual void setupFonts() = 0;
    virtual void update(float dt);
    virtual void draw();
    void setDebug(bool debug);
    bool isDebugging();
    entityx::EntityManager& getEntityManager();
    entityx::SystemManager& getSystemManager();
    std::vector<std::pair<std::string, bool>> getLabelTree();
    void showNode(std::string& label);
    void hideNode(std::string& label);
    void addChild(entityx::Entity& parent, entityx::Entity& child);
    entityx::Entity createNode(const std::string& label, const ci::vec3 position, bool show = true);
    entityx::Entity createTextElement(const std::string& label,
                                      const ci::vec3& baseline,
                                      const std::string& fontStyle,
                                      const std::string& text,
                                      const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
    entityx::Entity createTextElement(const std::string& label,
                                      const ci::Rectf& fitRect,
                                      const std::string& fontStyle,
                                      const std::string& text,
                                      const ci::gl::SdfText::DrawOptions& options = ci::gl::SdfText::DrawOptions());
    entityx::Entity createImageElement(const std::string& label,
                                       const ci::vec3& position,
                                       ci::gl::Texture2dRef imageTex,
                                       ci::ColorA tint = ci::Color::white());
    entityx::Entity createFboElement(const std::string& label,
                                     const ci::vec3& position,
                                     const ci::vec2& size,
                                     const ci::gl::Fbo::Format& format = ci::gl::Fbo::Format());
    entityx::Entity createRectangleElement(const std::string& label,
                                           const ci::vec3& position,
                                           const ci::vec2& size,
                                           const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRectangleElement(const std::string& label,
                                           const ci::vec3& position,
                                           const ci::geom::Rect& rect,
                                           const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRoundedRectElement(const std::string& label,
                                             const ci::vec3& position,
                                             const ci::vec2& size,
                                             float radius,
                                             const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRoundedRectElement(const std::string& label,
                                             const ci::vec3& position,
                                             const ci::geom::RoundedRect& rect,
                                             const ci::ColorA& color = ci::Color::white());
    entityx::Entity createRectangleButton(const std::string& label,
                                          const ci::vec3& position,
                                          const ci::vec2& size,
                                          const ci::ColorA& color = ci::Color::white());
    entityx::Entity createImageButton(const std::string& label,
                                      const ci::vec3& position,
                                      ci::gl::Texture2dRef imageTex,
                                      ci::ColorA tint = ci::Color::white());

   protected:
    entityx::EntityManager mEntities;
    entityx::EventManager mEvents;
    entityx::SystemManager mSystems;
    ci::TimelineRef mContentTimeline;
    bool mDebug;
};
}  // namespace ecs
}  // namespace sitara

#endif