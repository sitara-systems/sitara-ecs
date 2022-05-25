// Copyright 2022 Nathan Lachenmyer

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "Ecs.h"
#include <Windows.h>

class MouseSystemExampleApp : public ci::app::App {
   public:
    MouseSystemExampleApp();
    void setup() override;
    void update() override;
    void draw() override;
    void keyDown(ci::app::KeyEvent event) override;

    entityx::EventManager mEvents;
    entityx::EntityManager mEntities;
    entityx::SystemManager mSystems;
};

MouseSystemExampleApp::MouseSystemExampleApp() : mEntities(mEvents), mSystems(mEntities, mEvents) {}

void MouseSystemExampleApp::setup() {
    auto nativeWindow = static_cast<HWND>(ci::app::getWindow()->getNative());
    ::SetForegroundWindow(nativeWindow);
    ::SetFocus(nativeWindow);

    sitara::ecs::configureSystems(mSystems);

    mSystems.add<sitara::ecs::MouseSystem>(mEntities);
    mSystems.add<sitara::ecs::TransformSystem>();
    mSystems.configure();

    for (int i = 0; i < 8; i++) {
        auto pos = ci::vec2(ci::randInt(0, ci::app::getWindowSize().x), ci::randInt(0, ci::app::getWindowSize().y));
        auto e = mEntities.create();
        e.assign<sitara::ecs::Transform>(ci::vec3(pos, 0));
        auto geom = sitara::ecs::geometry::createCircle(50);
        e.assign<sitara::ecs::Clickable2D>(geom);
        e.assign<sitara::ecs::Geometry>(geom, ci::Color(1.0f, 0.0f, 0.0f));
    }

    for (int i = 0; i < 8; i++) {
        auto pos = ci::vec2(ci::randInt(0, ci::app::getWindowSize().x), ci::randInt(0, ci::app::getWindowSize().y));
        auto e = mEntities.create();
        e.assign<sitara::ecs::Transform>(ci::vec3(pos, 0));
        auto geom = sitara::ecs::geometry::createRect(ci::vec2(50, 50));
        e.assign<sitara::ecs::Clickable2D>(geom);
        e.assign<sitara::ecs::Geometry>(geom,
                                        ci::Color(0.0f, 1.0f, 0.0f));
    }

    for (int i = 0; i < 8; i++) {
        auto pos = ci::vec2(ci::randInt(0, ci::app::getWindowSize().x), ci::randInt(0, ci::app::getWindowSize().y));
        auto e = mEntities.create();
        e.assign<sitara::ecs::Transform>(ci::vec3(pos, 0));
        auto geom = sitara::ecs::geometry::createRoundedRect(ci::vec2(50, 50), 10);
        e.assign<sitara::ecs::Clickable2D>(geom);
        e.assign<sitara::ecs::Geometry>(geom,
                                        ci::Color(0.0f, 0.0f, 1.0f));
    }
}

void MouseSystemExampleApp::update() {
    mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);  // doesnt actually use time
}

void MouseSystemExampleApp::draw() {
    ci::gl::clear(ci::Color(0, 0, 0));
    ci::gl::enableAlphaBlending();
    ci::gl::setMatricesWindowPersp(getWindowSize());

    ci::gl::ScopedDepth depth(true);

    entityx::ComponentHandle<sitara::ecs::Transform> transform;
    entityx::ComponentHandle<sitara::ecs::Geometry> geometry;

    for (auto entity : mEntities.entities_with_components(transform, geometry)) {
        ci::gl::ScopedModelMatrix mat;
        ci::gl::multModelMatrix(transform->getWorldTransform());
        geometry->draw();
    }
}

void MouseSystemExampleApp::keyDown(ci::app::KeyEvent event) {
    if (event.getCode() == ci::app::KeyEvent::KEY_ESCAPE) {
        quit();
    }
}

CINDER_APP(MouseSystemExampleApp, ci::app::RendererGl, [=](cinder::app::App::Settings* settings) {
    settings->setTitle("Mouse System Test Application");
    settings->setWindowSize(1920, 1080);
    settings->setConsoleWindowEnabled();
    settings->setHighDensityDisplayEnabled(false);
});
