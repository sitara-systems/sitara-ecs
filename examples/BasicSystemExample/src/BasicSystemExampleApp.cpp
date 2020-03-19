#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace sitara {
	struct Sphere {
		Sphere(float diameter, Color color) :
			mRadius(diameter / 2.0f),
			mColor(color)
		{
			auto mesh = geom::Sphere().radius(mRadius).subdivisions(16);
			auto glColor = geom::Constant(geom::COLOR, mColor);
			auto lambert = gl::ShaderDef().lambert().color();
			auto shader = gl::getStockShader(lambert);
			mSphere = gl::Batch::create(mesh >> glColor, shader);
		}

		float mRadius;
		Color mColor;
		gl::BatchRef mSphere;
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
}

class BasicSystemExampleApp : public App {
  public:
	  BasicSystemExampleApp();
	  void setup() override;
	  void mouseDown( MouseEvent event ) override;
	  void update() override;
	  void draw() override;
	  entityx::Entity createSolarSystem(const ci::vec3 &center);
	  entityx::Entity createPlanetoid(entityx::Entity& sun);

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
	mSystems.add<sitara::RotationSystem>();
	mSystems.configure();

	createSolarSystem(vec3(getWindowCenter(), 0.0f));
}

void BasicSystemExampleApp::mouseDown( MouseEvent event ) {
}

void BasicSystemExampleApp::update() {
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0); // doesnt actually use time
	mSystems.update<sitara::RotationSystem>(1.0 / 60.0);
}

void BasicSystemExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();
	gl::setMatricesWindowPersp(getWindowSize());

	gl::ScopedDepth depth(true);

	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<sitara::Sphere> sphere;

	for (auto entity : mEntities.entities_with_components(transform, sphere)) {
		gl::ScopedModelMatrix mat;
		gl::multModelMatrix(transform->getWorldTransform());
		sphere->mSphere->draw();
	}
}

entityx::Entity BasicSystemExampleApp::createSolarSystem(const ci::vec3 &center) {
	auto sun = mEntities.create();
	sun.assign<sitara::Sphere>(50.0f, Color(1.0, 0.5, 0.0));
	sun.assign<sitara::ecs::Transform>(center);

	for (auto i = 0; i < 15; i ++) {
		createPlanetoid(sun);
	}

	return sun;
}

entityx::Entity BasicSystemExampleApp::createPlanetoid(entityx::Entity& sun) {
	auto planet = mEntities.create();
	auto size = randFloat(20.0f, 40.0f);
	auto distance = randFloat(100.0f, 360.0f);
	auto theta = randFloat(2.0 * M_PI);
	auto planet_pos = vec3(cos(theta) * distance, sin(theta) * distance, 0.0f);
	planet.assign<sitara::ecs::Transform>(planet_pos, vec3(1.0f), -planet_pos);
	mSystems.system<sitara::ecs::TransformSystem>()->attachChild(sun, planet);
	planet.assign<sitara::Sphere>(size, Color(0.0, 1.0, 1.0));
	planet.assign<sitara::Rotation>(vec3(0.0, 0.0, 1.0), randFloat(0.5));

	auto moon_pos = vec3(size, size, 0.0f) * randFloat(1.0f, 1.5f);
	auto moon = mEntities.create();
	moon.assign<sitara::ecs::Transform>(moon_pos, vec3(1.0f), -moon_pos);
	moon.assign<sitara::Sphere>(size * 0.33f, Color(1.0, 1.0, 1.0));
	mSystems.system<sitara::ecs::TransformSystem>()->attachChild(planet, moon);
	moon.assign<sitara::Rotation>(vec3(0.0, 0.0, 1.0), randFloat(1.0));

	return planet;
}

CINDER_APP( BasicSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setWindowSize(1920, 1080); })