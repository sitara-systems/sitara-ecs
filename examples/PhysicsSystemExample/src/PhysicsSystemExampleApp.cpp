#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PhysicsSystemExampleApp : public App {
  public:
	PhysicsSystemExampleApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void createUserInterface();
	void createWorld();
	void resetWorld();

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	ci::params::InterfaceGlRef mParams;
	std::vector<std::string> mElasticEnums = { "On", "Off" };
	int mElasticSelect = 1;
	std::vector<std::string> mFrictionEnums = { "On", "Off" };
	int mFrictionSelect = 0;
	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;
};

PhysicsSystemExampleApp::PhysicsSystemExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void PhysicsSystemExampleApp::setup() {
	ci::app::setFrameRate(60);
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.configure();

	physics->setGravity(vec3(0.0, -10.0, 0.0));

	ci::fs::path relativePath = "../../../assets";
	ci::fs::path absolutePath = ci::fs::canonical(relativePath);
	addAssetDirectory(absolutePath);

	createWorld();

	// setup camera orientation vectors
	mCameraDistance = 400.0;
	mEye = mCameraDistance * glm::normalize(vec3(1, 0.25, 1));
	mCenter = vec3(0.0f, 0.0f, 0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);

	createUserInterface();
}

void PhysicsSystemExampleApp::mouseDown( MouseEvent event ) {
}

void PhysicsSystemExampleApp::update() {
	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);
}

void PhysicsSystemExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();

	gl::setMatrices(mCamera);

	// Enable the depth buffer.
	gl::enableDepthRead();
	gl::enableDepthWrite();

	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::Geometry> geom;

	for (auto entity : mEntities.entities_with_components(body, geom)) {
		gl::pushModelMatrix();
		gl::setModelMatrix(body->getWorldTransform());
		geom->draw();
		gl::popModelMatrix();
	}


	gl::disableDepthWrite();
	gl::disableDepthRead();

	mParams->draw();
}

void PhysicsSystemExampleApp::createUserInterface() {
	mParams = ci::params::InterfaceGl::create("Physics System Example", vec2(250, 350));
	//mParams->addParam("Scene Rotation", &mSceneRotation, "opened=1"); // the opened param opens the panel per default
	mParams->addText("View Controls");
	mParams->addButton("To Top View", [=]() {
		mEye = mCameraDistance * glm::normalize(vec3(0, 1.0, 0));
		mCenter = vec3(0.0f, 0.0f, 0.0f);
		mUp = vec3(0.0f, 0.0f, 1.0f);
		mCamera.lookAt(mEye, mCenter, mUp);
	});
	mParams->addButton("To Side View", [=]() {
		mEye = mCameraDistance * glm::normalize(vec3(0, 0, 1.0));
		mCenter = vec3(0.0f, 0.0f, 0.0f);
		mUp = vec3(0.0f, 1.0f, 0.0f);
		mCamera.lookAt(mEye, mCenter, mUp);
	});
	mParams->addButton("To Perspective View", [=]() {
		mEye = mCameraDistance * glm::normalize(vec3(1, 0.25, 1));
		mCenter = vec3(0.0f, 0.0f, 0.0f);
		mUp = vec3(0.0f, 1.0f, 0.0f);
		mCamera.lookAt(mEye, mCenter, mUp);
	});
	mParams->addButton("Toggle Wireframes", [=]() {
		entityx::ComponentHandle<sitara::ecs::Geometry> geom;
		for (auto entity : mEntities.entities_with_components(geom)) {
			if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::SPHERE) {
				entity.remove<sitara::ecs::Geometry>();
				entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWireSphere(5.0), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
			}
			else if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::WIRE_SPHERE) {
				entity.remove<sitara::ecs::Geometry>();
				entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(5.0), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
			}
		}
	});

	mParams->addText("Simulation Controls");
	mParams->addButton("Reset Simulation", [=]() {
		resetWorld();
	});
	mParams->addParam("Set Elasticity", mElasticEnums, &mElasticSelect).updateFn([=]() {
		entityx::ComponentHandle<sitara::ecs::RigidBody> body;
		if (mElasticSelect == 0) {
			for (auto entity : mEntities.entities_with_components(body)) {
				body->setElasticity(1.0);
			}
		}
		else if (mElasticSelect == 1) {
			for (auto entity : mEntities.entities_with_components(body)) {
				body->setElasticity(0.0);
			}
		}
	});
	mParams->addParam("Set Friction", mFrictionEnums, &mFrictionSelect).updateFn([=]() {
		entityx::ComponentHandle<sitara::ecs::RigidBody> body;
		if (mFrictionSelect == 0) {
			for (auto entity : mEntities.entities_with_components(body)) {
				body->setFriction(1.0);
			}
		}
		else if (mFrictionSelect == 1) {
			for (auto entity : mEntities.entities_with_components(body)) {
				body->setFriction(0.0);
			}
		}
	});
}

void PhysicsSystemExampleApp::createWorld() {
	auto ground = mEntities.create();
	vec3 size = vec3(500, 1, 500);
	vec3 position = vec3(0, 0, 0);
	ground.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createBox(size, 0.0, position));
	ground.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCube(size), Color(1.0, 1.0, 1.0));

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150));
		auto ball = mEntities.create();
		float radius = 5.0;
		ball.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createSphere(radius, 50.0, pos));
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
	}
}

void PhysicsSystemExampleApp::resetWorld() {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::ecs::Geometry> geom;

	for (auto entity : mEntities.entities_with_components(body, geom)) {
		if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::SPHERE) {
			body->resetBody(ci::vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150)));
		}
	}
}

CINDER_APP( PhysicsSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setWindowSize(1280, 720); })
