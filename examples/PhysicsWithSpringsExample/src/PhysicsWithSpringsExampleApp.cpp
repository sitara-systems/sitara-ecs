#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PhysicsWithSpringsExampleApp : public App {
  public:
	PhysicsWithSpringsExampleApp();
	void setup() override;
	void update() override;
	void draw() override;

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	int mBallMaterialId;

	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;
};

PhysicsWithSpringsExampleApp::PhysicsWithSpringsExampleApp() :
	mEntities(mEvents), mSystems(mEntities, mEvents) {
}

void PhysicsWithSpringsExampleApp::setup() {
	ci::app::setFrameRate(60.0f);
	sitara::ecs::configureSystems(mSystems);
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.configure();

	physics->setGravity(vec3(0.0, -9.8f, 0.0));

	{
		int groundMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(0, 0, 0);
		physx::PxMaterial* groundMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(groundMaterialId);

		auto ground = mEntities.create();
		vec3 halfSize = vec3(500, 1, 250);
		vec3 position = vec3(0, 0, 0);

		auto body = ground.assign<sitara::ecs::StaticBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createStaticBody(position));
		body->attachBox(halfSize, groundMaterial);
		ground.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(halfSize), Color(1.0, 1.0, 1.0));
	}

	mBallMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(1.0f, 1.0f, 1.0f);
	physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(mBallMaterialId);

	for (int i = 0; i < 7; i++) {
		float x = (i + 1) * ci::app::getWindowWidth() / 9.0f - 0.5f*ci::app::getWindowWidth();
		auto pos = vec3(x, 200.0f, 0.0f);
		auto ball = mEntities.create();
		float radius = 15.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachSphere(radius, ballMaterial);
		physics->createSpring(body, pos + ci::vec3(0, 300, 0), ci::randFloat(0.1f, 1.0f), ci::randFloat(0.1f, 1.0f));
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
	}

	mCameraDistance = 500;
	mEye = vec3(0.5f * ci::app::getWindowWidth(), 300.0f, mCameraDistance);
	mCenter = vec3(0.0f, 0.0f, 0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, ci::app::getWindowAspectRatio(), 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);
}
 

void PhysicsWithSpringsExampleApp::update() {
	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void PhysicsWithSpringsExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::setMatrices(mCamera);

	gl::enableDepthRead();
	gl::enableDepthWrite();

	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<sitara::ecs::Geometry> geom;

	for (auto entity : mEntities.entities_with_components(transform, geom)) {
		gl::pushModelMatrix();
		gl::setModelMatrix(transform->getWorldTransform());
		geom->draw();
		gl::popModelMatrix();
	}

	gl::disableDepthWrite();
	gl::disableDepthRead();
}

CINDER_APP( PhysicsWithSpringsExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {
	settings->setTitle("Sitara Systems Template Application");
	settings->setWindowSize(1920, 1080);
	settings->setConsoleWindowEnabled();
	settings->setHighDensityDisplayEnabled(false);
});
