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
			auto mesh = ci::geom::Sphere().radius(mRadius).subdivisions(16);
			auto glColor = ci::geom::Constant(ci::geom::COLOR, mColor);
			auto lambert = ci::gl::ShaderDef().lambert().color();
			auto shader = ci::gl::getStockShader(lambert);
			mSphere = ci::gl::Batch::create(mesh >> glColor, shader);
		}

		float mRadius;
		Color mColor;
		gl::BatchRef mSphere;
	};

	struct Box {
		Box(vec3 size, Color color) :
			mDimensions(size),
			mColor(color)
		{
			auto mesh = geom::Cube().size(mDimensions);
			auto glColor = geom::Constant(geom::COLOR, mColor);
			auto lambert = gl::ShaderDef().lambert().color();
			auto shader = gl::getStockShader(lambert);
			mBox = gl::Batch::create(mesh >> glColor, shader);
		}

		vec3 mDimensions;
		Color mColor;
		gl::BatchRef mBox;
	};

}

class PhysicsSystemExampleApp : public App {
  public:
	PhysicsSystemExampleApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	ci::CameraPersp mCamera;
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

	auto ground = mEntities.create();
	vec3 position = vec3(0, -56, 0);
	vec3 size = vec3(500, 1, 500);
	auto g = ground.assign<sitara::ecs::RigidBody>(sitara::ecs::BOX, vec3(0, -56.0, 0), size);
	ground.assign<sitara::Box>(size, Color(1.0, 1.0, 1.0));
	g->setElasticity(1.0);

	for (int i = 0; i < 50; i++) {
		auto ps = vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150));
		auto ball = mEntities.create();
		vec3 size = vec3(10, 10, 10);
		auto rigidBody = ball.assign<sitara::ecs::RigidBody>(sitara::ecs::SPHERE, ps, size, 1.0);
		rigidBody->setElasticity(1.0);
		ball.assign<sitara::Sphere>(size.x, Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
	}

	// setup camera orientation vectors
	float cameraDistance = 400.0;
	vec3 eye = cameraDistance * glm::normalize(vec3(1, 0.25, 1));
	vec3 center = vec3(0.0f, 0.0f, 0.0f);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 10000.0f);
	mCamera.lookAt(eye, center, up);
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
	entityx::ComponentHandle<sitara::Sphere> sphere;
	entityx::ComponentHandle<sitara::Box> box;

	for (auto entity : mEntities.entities_with_components(body, sphere)) {
		gl::pushModelMatrix();
		gl::setModelMatrix(body->getWorldTransform());
		sphere->mSphere->draw();
		gl::popModelMatrix();
	}

	for (auto entity : mEntities.entities_with_components(body, box)) {
		gl::pushModelMatrix();
		gl::translate(body->getPosition());
		box->mBox->draw();
		gl::popModelMatrix();
	}

	gl::disableDepthWrite();
	gl::disableDepthRead();
}

CINDER_APP( PhysicsSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setWindowSize(1280, 720); })
