#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PhysicsGeometryExampleApp : public App {
  public:
	PhysicsGeometryExampleApp();
	void setup() override;
	void update() override;
	void draw() override;
	void createShape(int select);
	void mouseDown(MouseEvent event) override;

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;

	std::vector<int> mShapes = { sitara::ecs::geometry::BOX,
		sitara::ecs::geometry::CAPSULE,
		sitara::ecs::geometry::SPHERE
	};

	int mShapeSelect = 0;
};

PhysicsGeometryExampleApp::PhysicsGeometryExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void PhysicsGeometryExampleApp::setup() {
	sitara::ecs::configureSystems(mSystems);
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.configure();

	physics->registerMaterial(1.0f, 1.0f, 1.0f);

	createShape(mShapeSelect);

	// setup camera orientation vectors
	float center = 0.0f;
	mCameraDistance = 800;// 692;
	mEye = vec3(0, 250.0f, -mCameraDistance);
	mCenter = vec3(0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, 1.0f, 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);
}

void PhysicsGeometryExampleApp::update() {
	mSystems.update_all(1.0f / 60.0f);
	gl::rotate(0.2f * ci::app::getElapsedSeconds(), ci::vec3(0, 1, 0));
}

void PhysicsGeometryExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::setMatrices(mCamera);

	gl::enableDepth();

	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<sitara::ecs::Geometry> geom;

	for (auto entity : mEntities.entities_with_components(transform, geom)) {
		gl::pushModelMatrix();
		gl::setModelMatrix(transform->getWorldTransform());
		geom->draw();
		ci::Color(0, 0, 1);
		gl::popModelMatrix();
	}
}

void PhysicsGeometryExampleApp::createShape(int select) {
	float radius = 149.0;
	float height = 149.0f;
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body;

	auto floor = mEntities.create();
	ci::vec3 floorDimensions = ci::vec3(300, 1, 300);
	floor.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(floorDimensions), Color(0.5f, 0.5f, 0.5f));
	body = floor.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(ci::vec3(0, -150, 0)));
	body->attachBox(floorDimensions, mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(0));

	auto shape = mEntities.create();
	switch (select) {
	case 0:
		shape.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(ci::vec3(radius)), Color(0.5f, 0.0f, 0.5f));
		body = shape.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(ci::vec3(0)));
		body->attachBox(radius, mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(0));
		break;
	case 1:
		shape.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCapsule(radius, height), Color(0.5f, 0.0f, 0.5f));
		body = shape.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(ci::vec3(0)));
		body->attachCapsule(25, 124, mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(0));
		break;
	case 2:
		shape.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(0.5f, 0.0f, 0.5f));
		body = shape.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(ci::vec3(0)));
		body->attachSphere(radius, mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(0));
		break;
	}

}

void PhysicsGeometryExampleApp::mouseDown(MouseEvent event) {
	mEntities.reset();
	mShapeSelect++;
	mShapeSelect = mShapeSelect % 3;
	createShape(mShapeSelect);
}

CINDER_APP(PhysicsGeometryExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {
	settings->setTitle("Physics Geometry Example");
	settings->setConsoleWindowEnabled();
	settings->setWindowSize(ci::vec2(800, 800));
});