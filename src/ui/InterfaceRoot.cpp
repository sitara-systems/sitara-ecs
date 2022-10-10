#include "ui/InterfaceRoot.h"
#include "Ecs.h"

#ifdef USING_SDFTEXT

using namespace sitara::ecs;

InterfaceRoot::InterfaceRoot() : mEntities(mEvents), mSystems(mEntities, mEvents), mDebug(false) {
    setupSystems();
    mContentTimeline = ci::Timeline::create();
}

InterfaceRoot::~InterfaceRoot() {}

entityx::EntityManager& InterfaceRoot::getEntityManager() {
    return mEntities;
}

entityx::SystemManager& InterfaceRoot::getSystemManager() {
    return mSystems;
}

std::vector<std::pair<std::string, bool>> InterfaceRoot::getLabelTree() {
    return mSystems.system<sitara::ecs::TransformSystem>()->getLabelTree(mEntities);
}

void InterfaceRoot::showNode(std::string& label) {
    TransformHandle selectedNode = mSystems.system<sitara::ecs::TransformSystem>()->getNodeByLabel(mEntities, label);
    if (selectedNode.valid()) {
        selectedNode->show();
    }
}

void InterfaceRoot::hideNode(std::string& label) {
    TransformHandle selectedNode = mSystems.system<sitara::ecs::TransformSystem>()->getNodeByLabel(mEntities, label);
    if (selectedNode.valid()) {
        selectedNode->hide();
    }
}

void sitara::ecs::InterfaceRoot::fadeOutElement(entityx::Entity& fadeOutEntity,
                                                const sitara::ecs::Tween::Options& fadeOutOptions) {
    if (fadeOutEntity.valid()) {
        mSystems.system<sitara::ecs::TimelineSystem>()->tweenAlpha(fadeOutEntity, 0.0f, fadeOutOptions);
    }
}

void sitara::ecs::InterfaceRoot::fadeOutElement(const std::string& fadeOutLabel,
                                                const sitara::ecs::Tween::Options& fadeOutOptions) {
    auto transformSystem = mSystems.system<sitara::ecs::TransformSystem>();
    sitara::ecs::TransformHandle fadeOutHandle = transformSystem->getNodeByLabel(mEntities, fadeOutLabel);
    entityx::Entity fadeOutEntity = entityx::Entity();
    if (fadeOutHandle.valid()) {
        fadeOutEntity = fadeOutHandle.entity();
    }
    fadeOutElement(fadeOutEntity, fadeOutOptions);
}

void sitara::ecs::InterfaceRoot::fadeOutElement(const std::string& fadeOutLabel) {
    sitara::ecs::Tween::Options fadeOutOptions(0.5f);
    fadeOutOptions.mFinishFn = [&](entityx::Entity& e) {
        TransformHandle transform = e.component<sitara::ecs::Transform>();
        transform->hide();
    };

    fadeOutElement(fadeOutLabel, fadeOutOptions);
}

void sitara::ecs::InterfaceRoot::fadeInElement(entityx::Entity& fadeInEntity,
                                               const sitara::ecs::Tween::Options& fadeInOptions) {
    if (fadeInEntity.valid()) {
        mSystems.system<sitara::ecs::TimelineSystem>()->tweenAlpha(fadeInEntity, 1.0f, fadeInOptions);
    }
}

void sitara::ecs::InterfaceRoot::fadeInElement(const std::string& fadeInLabel,
                                               const sitara::ecs::Tween::Options& fadeInOptions) {
    auto transformSystem = mSystems.system<sitara::ecs::TransformSystem>();
    sitara::ecs::TransformHandle fadeInHandle = transformSystem->getNodeByLabel(mEntities, fadeInLabel);
    entityx::Entity fadeInEntity = entityx::Entity();
    if (fadeInHandle.valid()) {
        fadeInEntity = fadeInHandle.entity();
    }
    fadeInElement(fadeInEntity, fadeInOptions);
}

void sitara::ecs::InterfaceRoot::fadeInElement(const std::string& fadeInLabel) {
    sitara::ecs::Tween::Options fadeInOptions(0.5f, 0.5f);
    fadeInOptions.mStartFn = [&](entityx::Entity& e) {
        TransformHandle transform = e.component<sitara::ecs::Transform>();
        transform->show();
    };

    fadeInElement(fadeInLabel, fadeInOptions);
}

void InterfaceRoot::crossfadeElements(const std::string& fadeOutLabel,
                                      const std::string& fadeInLabel,
                                      const sitara::ecs::Tween::Options& fadeOutOptions,
                                      const sitara::ecs::Tween::Options& fadeInOptions) {
    fadeOutElement(fadeOutLabel, fadeOutOptions);
    fadeInElement(fadeInLabel, fadeInOptions);
}

void sitara::ecs::InterfaceRoot::crossfadeElements(const std::string& fadeOutLabel, const std::string& fadeInLabel) {
    sitara::ecs::Tween::Options fadeOutOptions(0.5f);
    fadeOutOptions.mFinishFn = [&](entityx::Entity& e) {
        TransformHandle transform = e.component<sitara::ecs::Transform>();
        transform->hide();
    };

    sitara::ecs::Tween::Options fadeInOptions(0.5f, 0.5f);
    fadeInOptions.mStartFn = [&](entityx::Entity& e) {
        TransformHandle transform = e.component<sitara::ecs::Transform>();
        transform->show();
    };

    crossfadeElements(fadeOutLabel, fadeInLabel, fadeOutOptions, fadeInOptions);
}

void InterfaceRoot::setupSystems() {
    sitara::ecs::configureSystems(mSystems);
    mSystems.add<sitara::ecs::TransformSystem>();
    mSystems.add<sitara::ecs::MouseSystem>(mEntities, mSystems);
    mSystems.add<sitara::ecs::TextSystem>();
    mSystems.add<sitara::ecs::FboSystem>();
    mSystems.add<sitara::ecs::StateSystem>();
    mSystems.add<sitara::ecs::TimelineSystem>();
    mSystems.configure();

    mSystems.system<sitara::ecs::TransformSystem>()->enableDepthSort(true);
}

void InterfaceRoot::update(float dt) {
    mContentTimeline->stepTo(static_cast<float>(ci::app::getElapsedSeconds()));
    mSystems.update<sitara::ecs::TimelineSystem>(dt);
    mSystems.update<sitara::ecs::TransformSystem>(dt);
    mSystems.update<sitara::ecs::FboSystem>(dt);
}

void InterfaceRoot::draw() {
}

void InterfaceRoot::setDebug(bool debug) {
    mDebug = debug;
}

bool InterfaceRoot::isDebugging() {
    return mDebug;
}

void InterfaceRoot::addChild(entityx::Entity& parent, entityx::Entity& child) {
    mSystems.system<sitara::ecs::TransformSystem>()->attachChild(parent, child);    
}

entityx::Entity InterfaceRoot::createNode(const std::string& label, const ci::vec3 position, bool show) {
    entityx::Entity nodeEntity = mEntities.create();
    sitara::ecs::TransformHandle transformHandle = nodeEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);
    if (show) {
        transformHandle->show();
    } else {
        transformHandle->hide();
        transformHandle->mTintColor = ci::ColorA(1, 1, 1, 0);
    }

    nodeEntity.assign<sitara::ecs::LogicState>(NodeState::HIDE);
    nodeEntity.assign<sitara::ecs::Tween>();
    return nodeEntity;
}

entityx::Entity InterfaceRoot::createTextElement(const std::string& label,
                                                              const ci::vec3& baseline,
                                                              const std::string& fontStyle,
                                                              const std::string& text,
                                                              const ci::gl::SdfText::DrawOptions& options) {
    entityx::Entity textEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = textEntity.assign<sitara::ecs::Transform>(baseline);
    transformHandle->setLabel(label);

    auto textSystem = mSystems.system<sitara::ecs::TextSystem>();
    textSystem->addTextComponent(textEntity, fontStyle, text, baseline, options);

    textEntity.assign<sitara::ecs::Tween>();

    return textEntity;
}

entityx::Entity InterfaceRoot::createTextElement(const std::string& label,
                                                              const ci::Rectf& fitRect,
                                                              const std::string& fontStyle,
                                                              const std::string& text,
                                                              const ci::gl::SdfText::DrawOptions& options) {
    entityx::Entity textEntity = mEntities.create();

    auto textSystem = mSystems.system<sitara::ecs::TextSystem>();
    sitara::ecs::TextHandle textHandle = textSystem->addTextComponent(textEntity, fontStyle, text, fitRect, options);

    sitara::ecs::TransformHandle transformHandle = textEntity.assign<sitara::ecs::Transform>();
    transformHandle->setLabel(label);

    textEntity.assign<sitara::ecs::Tween>();

    return textEntity;
}

entityx::Entity InterfaceRoot::createImageElement(const std::string& label,
                                                  const ci::vec3& position,
                                                  ci::gl::Texture2dRef imageTex,
                                                  ci::ColorA tint) {
    entityx::Entity imageEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = imageEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);
    ci::geom::Rect imageShape = sitara::ecs::geometry::createRect(imageTex->getSize());

    sitara::ecs::GeometryHandle imageGeometryComponent = imageEntity.assign<sitara::ecs::Geometry>(imageShape, imageTex, tint);

    imageEntity.assign<sitara::ecs::Tween>();

    return imageEntity;
}

entityx::Entity InterfaceRoot::createFboElement(const std::string& label,
                                                const ci::vec3& position,
                                                const ci::vec2& size,
                                                const ci::gl::Fbo::Format& format) {
    ci::gl::Texture2dRef texture = ci::gl::Texture2d::create(size.x, size.y);
    entityx::Entity fboEntity = createImageElement(label, position, texture);
    fboEntity.assign<sitara::ecs::Fbo>(size, format);
    return fboEntity;
}

entityx::Entity InterfaceRoot::createRectangleElement(const std::string& label,
                                                      const ci::vec3& position,
                                                      const ci::vec2& size,
                                                      const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = rectEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);

    ci::geom::Rect geom = sitara::ecs::geometry::createRect(size);
    rectEntity.assign<sitara::ecs::Geometry>(geom, color);
    rectEntity.assign<sitara::ecs::Tween>();

    return rectEntity;
}

entityx::Entity InterfaceRoot::createRectangleElement(const std::string& label,
                                                                   const ci::vec3& position,
                                                                   const ci::geom::Rect& rect,
                                                                   const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = rectEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);

    rectEntity.assign<sitara::ecs::Geometry>(rect, color);
    rectEntity.assign<sitara::ecs::Tween>();

    return rectEntity;
}

entityx::Entity InterfaceRoot::createRoundedRectElement(const std::string& label,
                                                                     const ci::vec3& position,
                                                                     const ci::vec2& size,
                                                                     float radius,
                                                                     const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = rectEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);

    ci::geom::RoundedRect rect = sitara::ecs::geometry::createRoundedRect(size, radius);
    rectEntity.assign<sitara::ecs::Geometry>(rect, color);
    rectEntity.assign<sitara::ecs::Tween>();

    return rectEntity;
}

entityx::Entity InterfaceRoot::createRoundedRectElement(const std::string& label,
                                                                     const ci::vec3& position,
                                                                     const ci::geom::RoundedRect& rect,
                                                                     const ci::ColorA& color) {
    entityx::Entity rectEntity = mEntities.create();

    sitara::ecs::TransformHandle transformHandle = rectEntity.assign<sitara::ecs::Transform>(position);
    transformHandle->setLabel(label);
    rectEntity.assign<sitara::ecs::Geometry>(rect, color);
    rectEntity.assign<sitara::ecs::Tween>();

    return rectEntity;
}

entityx::Entity InterfaceRoot::createRectangleButton(const std::string& label,
                                                                  const ci::vec3& position,
                                                                  const ci::vec2& size,
                                                                  const ci::ColorA& color) {
    entityx::Entity buttonEntity = createRectangleElement(label, position, size, color);
    ci::geom::Rect geom = sitara::ecs::geometry::createRect(size);

    buttonEntity.assign<sitara::ecs::Clickable2D>(geom);
    buttonEntity.assign<sitara::ecs::LogicState>(ButtonState::INACTIVE);

    return buttonEntity;
}

entityx::Entity InterfaceRoot::createImageButton(const std::string& label,
                                                              const ci::vec3& position,
                                                              ci::gl::Texture2dRef imageTex,
                                                              ci::ColorA tint) {
    entityx::Entity imageEntity = createImageElement(label, position, imageTex, tint);
    ci::geom::Rect geom = sitara::ecs::geometry::createRect(imageTex->getSize());
    imageEntity.assign<sitara::ecs::Clickable2D>(geom);
    imageEntity.assign<sitara::ecs::LogicState>(ButtonState::INACTIVE);

    return imageEntity;
}

#endif