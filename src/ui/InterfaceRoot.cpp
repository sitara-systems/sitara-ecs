#include "ui/InterfaceRoot.h"
#include "Ecs.h"

#ifdef USING_SDFTEXT

using namespace sitara::ecs;

InterfaceRoot::InterfaceRoot() : mEntities(mEvents), mSystems(mEntities, mEvents) {
    setupSystems();
}

InterfaceRoot::~InterfaceRoot() {}

entityx::EntityManager& InterfaceRoot::getEntityManager() {
    return mEntities;
}

entityx::SystemManager& InterfaceRoot::getSystemManager() {
    return mSystems;
}

void InterfaceRoot::setupSystems() {
    sitara::ecs::configureSystems(mSystems);
    mSystems.add<sitara::ecs::TransformSystem>();
    mSystems.add<sitara::ecs::MouseSystem>(mEntities);
    mSystems.add<sitara::ecs::TextSystem>();
    mSystems.configure();
}

void InterfaceRoot::update(float dt) {
    mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void InterfaceRoot::draw() {
    //ci::gl::clear(ci::Color(1.0f, 1.0f, 1.0f));
    //ci::gl::enableAlphaBlending();
    //ci::gl::setMatricesWindowPersp(ci::app::getWindowSize());

    //entityx::ComponentHandle<sitara::ecs::Transform> transform;
    //entityx::ComponentHandle<sitara::ecs::Geometry> geometry;
    //entityx::ComponentHandle<sitara::ecs::GlyphString> text;

    //for (auto entity : mEntities.entities_with_components(transform, geometry)) {
    //    ci::gl::ScopedDepth depth(true);
    //    ci::gl::ScopedModelMatrix mat;
    //    ci::gl::multModelMatrix(transform->getWorldTransform());
    //    geometry->draw();
    //}

    //auto textSystem = mSystems.system<sitara::ecs::TextSystem>();
    //for (auto entity : mEntities.entities_with_components(transform, text)) {
    //    ci::gl::ScopedModelMatrix mat;
    //    ci::gl::multModelMatrix(transform->getWorldTransform());

    //    textSystem->drawGlyphString(text);
    //}
}

void InterfaceRoot::addChild(entityx::Entity& parent, entityx::Entity& child) {
    mSystems.system<sitara::ecs::TransformSystem>()->attachChild(parent, child);    
}

entityx::Entity sitara::ecs::InterfaceRoot::createTextElement(const ci::vec3& position,
                                                              const std::string& fontStyle,
                                                              const std::string& text,
                                                              const ci::gl::SdfText::DrawOptions& options) {
    entityx::Entity textEntity = mEntities.create();

    textEntity.assign<sitara::ecs::Transform>(position);
    mSystems.system<sitara::ecs::TextSystem>()->addGlyphStringComponent(textEntity, fontStyle, text, options);
    return textEntity;
}

entityx::Entity InterfaceRoot::createImageElement(const ci::vec3& position, ci::gl::Texture2dRef imageTex) {
    entityx::Entity imageEntity = mEntities.create();
    imageEntity.assign<sitara::ecs::Transform>(position);
    ci::geom::Rect imageShape = sitara::ecs::geometry::createRect(imageTex->getSize());
    entityx::ComponentHandle<sitara::ecs::Geometry> imageGeometryComponent = imageEntity.assign<sitara::ecs::Geometry>(imageShape, imageTex);
    return imageEntity;
}

entityx::Entity InterfaceRoot::createFboElement(const ci::vec3& position, const ci::vec2& size, const ci::gl::Fbo::Format& format) {
    ci::gl::Texture2dRef texture = ci::gl::Texture2d::create(size.x, size.y);
    entityx::Entity fboEntity = createImageElement(position, texture);
    fboEntity.assign<sitara::ecs::Fbo>(size, format);
    return fboEntity;
}

entityx::Entity InterfaceRoot::createRectangleElement(const ci::vec3& position,
                                                      const ci::vec2& size,
                                                      const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    rectEntity.assign<sitara::ecs::Transform>(position);

    ci::geom::Rect geom = sitara::ecs::geometry::createRect(size);
    rectEntity.assign<sitara::ecs::Geometry>(geom, color);

    return rectEntity;
}

entityx::Entity sitara::ecs::InterfaceRoot::createRectangleElement(const ci::vec3& position,
                                                                   const ci::geom::Rect& rect,
                                                                   const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    rectEntity.assign<sitara::ecs::Transform>(position);
    rectEntity.assign<sitara::ecs::Geometry>(rect, color);

    return rectEntity;
}

entityx::Entity sitara::ecs::InterfaceRoot::createRoundedRectElement(const ci::vec3& position,
                                                                     const ci::vec2& size,
                                                                     float radius,
                                                                     const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    rectEntity.assign<sitara::ecs::Transform>(position);

    ci::geom::RoundedRect rect = sitara::ecs::geometry::createRoundedRect(size, radius);
    rectEntity.assign<sitara::ecs::Geometry>(rect, color);

    return rectEntity;
}

entityx::Entity sitara::ecs::InterfaceRoot::createRoundedRectElement(const ci::vec3& position,
                                                                     const ci::geom::RoundedRect& rect,
                                                                     const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    rectEntity.assign<sitara::ecs::Transform>(position);
    rectEntity.assign<sitara::ecs::Geometry>(rect, color);

    return rectEntity;
}

entityx::Entity sitara::ecs::InterfaceRoot::createRectangleButton(const ci::vec3& position,
                                                                  const ci::vec2& size,
                                                                  const ci::ColorA& color) {
    entityx::Entity buttonEntity = createRectangleElement(position, size, color);
    ci::geom::Rect geom = sitara::ecs::geometry::createRect(size);

    buttonEntity.assign<sitara::ecs::Clickable2D>(geom);

    return buttonEntity;
}

entityx::Entity sitara::ecs::InterfaceRoot::createImageButton(const ci::vec3& position, ci::gl::Texture2dRef imageTex) {
    entityx::Entity imageEntity = createImageElement(position, imageTex);
    ci::geom::Rect geom = sitara::ecs::geometry::createRect(imageTex->getSize());
    imageEntity.assign<sitara::ecs::Clickable2D>(geom);

    return imageEntity;
}

#endif