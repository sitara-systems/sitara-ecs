#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Sun {
	Sun() = default;
};

struct Circle {
	Circle() = default;
	Circle(float radius, const ci::Color &color = ci::Color(1.0f, 1.0f, 1.0f))
	: mRadius(radius),
	mColor(color) {
	}

	float mRadius = 12.0f;
	ci::Color mColor = ci::Color(1.0f, 1.0f, 1.0f);
};

struct Rotation {
	Rotation(vec3 axis, float revs_per_second) : mAxis(axis), mRotationalSpeed(revs_per_second * 2.0 * M_PI) {

	}

	vec3 mAxis;
	float mRotationalSpeed;
};

struct RotationSystem : public entityx::System<RotationSystem> {
	void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
		entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;
		entityx::ComponentHandle<Rotation> rotationHandle;

		for (entityx::Entity e : entities.entities_with_components(transformHandle, rotationHandle)) {
			transformHandle->mOrientation *= glm::angleAxis<float>(rotationHandle->mRotationalSpeed * dt, rotationHandle->mAxis);
			glm::normalize(transformHandle->mOrientation);
		};
	};
};

class BasicSystemExampleApp : public App {
  public:
	  BasicSystemExampleApp();
	  void setup() override;
	  void mouseDown( MouseEvent event ) override;
	  void update() override;
	  void draw() override;
	  entityx::Entity createSolarSystem(entityx::EntityManager &entities, const ci::vec3 &center);
	  entityx::Entity createPlanetoid(entityx::EntityManager &entities);

	  entityx::EntityManager mEntities;
	  entityx::EventManager mEvents;
	  entityx::SystemManager mSystems;
};

BasicSystemExampleApp::BasicSystemExampleApp() : 
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void BasicSystemExampleApp::setup() {
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.add<RotationSystem>();
	mSystems.configure();

	createSolarSystem(mEntities, vec3(getWindowCenter(), 0.0f));
}

void BasicSystemExampleApp::mouseDown( MouseEvent event ) {
}

void BasicSystemExampleApp::update() {
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0); // doesnt actually use time
	mSystems.update<RotationSystem>(1.0 / 60.0);
}

void BasicSystemExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();
	gl::setMatricesWindowPersp(getWindowSize());

	gl::ScopedDepth depth(true);

	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<Circle> circle;

	for (auto entity : mEntities.entities_with_components(transform, circle)) {
		gl::ScopedModelMatrix mat;
		gl::multModelMatrix(transform->getWorldTransform());
		gl::color(circle->mColor);
		gl::drawSolidCircle(vec2(0), circle->mRadius);
	}
}

entityx::Entity BasicSystemExampleApp::createSolarSystem(entityx::EntityManager &entities, const ci::vec3 &center) {
	auto sun = entities.create();
	sun.assign<Circle>(50.0f, Color(1.0, 0.5, 0.0));
	sun.assign<sitara::ecs::Transform>(sun, center);
	sun.assign<Sun>();

	for (auto i = 0; i < 15; i ++) {
		sitara::ecs::attachChild(sun, createPlanetoid(entities));
	}

	return sun;
}

entityx::Entity BasicSystemExampleApp::createPlanetoid(entityx::EntityManager &entities) {
	auto planet = entities.create();
	auto size = randFloat(20.0f, 40.0f);
	auto distance = randFloat(100.0f, 360.0f);
	auto theta = randFloat(2.0 * M_PI);
	auto planet_pos = vec3(cos(theta) * distance, sin(theta) * distance, 0.0f);
	planet.assign<sitara::ecs::Transform>(planet, planet_pos, vec3(1.0f), -planet_pos);
	planet.assign<Circle>(size, Color(0.0, 1.0, 1.0));
	planet.assign<Rotation>(vec3(0.0, 0.0, 1.0), randFloat(0.5));

	auto moon_pos = vec3(size, size, 0.0f) * randFloat(1.0f, 1.5f);
	auto moon = entities.create();
	moon.assign<sitara::ecs::Transform>(moon, moon_pos, vec3(1.0f), -moon_pos);
	moon.assign<Circle>(size * 0.33f, Color(1.0, 1.0, 1.0));
	sitara::ecs::attachChild(planet, moon);
	moon.assign<Rotation>(vec3(0.0, 0.0, 1.0), randFloat(1.0));

	return planet;
}

CINDER_APP( BasicSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setWindowSize(1920, 1080); })