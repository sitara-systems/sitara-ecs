#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InteractivePhysicsExampleApp : public App {
	public:
	InteractivePhysicsExampleApp();
	void setup() override;
	void update() override;
	void draw() override;
	void mouseMove(MouseEvent event) override;
	void createWorld();
	void createTarget();
	void updateTarget();

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;
	entityx::Entity mTargetHandle;

	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;

	enum LayerNames {
		OBSTACLES,
		TARGET
	};
};

InteractivePhysicsExampleApp::InteractivePhysicsExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void InteractivePhysicsExampleApp::setup() {
	sitara::ecs::configureSystems(mSystems);
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.configure();

	mCameraDistance = 1200.0;
	mEye = vec3(0, 0, mCameraDistance);
	mCenter = vec3(0.0f, 0.0f, 0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);

	createWorld();
	createTarget();
}

void InteractivePhysicsExampleApp::update() {
	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void InteractivePhysicsExampleApp::draw() {
	gl::clear(Color(1, 1, 1));
	gl::enableAlphaBlending();

	gl::setMatrices(mCamera);

	// Enable the depth buffer.
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

void InteractivePhysicsExampleApp::mouseMove(MouseEvent event) {
	if (mTargetHandle) {
		auto body = mTargetHandle.component<sitara::ecs::StaticBody>();
		body->setPosition(ci::vec3(event.getX() - 0.5f*ci::app::getWindowWidth(), 0.5f*ci::app::getWindowHeight() - event.getY(), 0));
	}
}

void InteractivePhysicsExampleApp::createWorld() {

	int ballMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(1.0f, 1.0f, 1.0f);
	physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(ballMaterialId);

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-600, 600), ci::Rand::randFloat(-600, 600), 0);
		auto ball = mEntities.create();
		float radius = 15.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachSphere(radius, ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);
	}

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-600, 600), ci::Rand::randFloat(-600, 600), 0);
		auto ball = mEntities.create();
		float halfEdge = 15.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachBox(ci::vec3(halfEdge), ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(ci::vec3(halfEdge)), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);
	}

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-600, 600), ci::Rand::randFloat(-600, 600), 0);
		auto ball = mEntities.create();
		float radius = 15.0;
		float height = 10.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachCapsule(radius, height, ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCapsule(radius, height), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);
	}
}

void InteractivePhysicsExampleApp::createTarget() {
	int ballMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(1.0f, 1.0f, 1.0f);
	physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(ballMaterialId);

	auto pos = vec3(0, 0, 0);
	mTargetHandle = mEntities.create();
	float radius = 25.0;
	auto body = mTargetHandle.assign<sitara::ecs::StaticBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createStaticBody(pos));
	body->attachSphere(radius, ballMaterial);
	mTargetHandle.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(1, 1, 1));
	mTargetHandle.assign<sitara::ecs::LogicalLayer>(LayerNames::TARGET);
}

CINDER_APP( InteractivePhysicsExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {
	settings->setTitle("Sitara Systems Template Application");
	settings->setWindowSize(1200, 1200);
	settings->setConsoleWindowEnabled();
	settings->setHighDensityDisplayEnabled(false);
});
