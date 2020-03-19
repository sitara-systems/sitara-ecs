#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FlockingBehaviorExampleApp : public App {
  public:
	FlockingBehaviorExampleApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void createUserInterface();
	void createWorld();
	void resetWorld();
	entityx::Entity createWall(float size, vec3 dimensions, vec3 u, vec3 v);

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;

	ci::params::InterfaceGlRef mParams;


	sitara::ecs::Units mUnits;

	enum LayerNames {
		WALLS,
		OBSTACLES,
		BOIDS
	};

};

FlockingBehaviorExampleApp::FlockingBehaviorExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void FlockingBehaviorExampleApp::setup() {
	ci::app::setFrameRate(60);

	sitara::ecs::configureSystem(mSystems);
	mSystems.add<sitara::ecs::BehaviorSystem>();
	mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.configure();

	mUnits = sitara::ecs::Units(10.0);

	mCameraDistance = mUnits.getPixelsfromMeters(75.0f);
	mEye = mCameraDistance * glm::normalize(vec3(1, 0.25, 1));
	mCenter = vec3(0.0f, 0.0f, 0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);

	createUserInterface();

	createWorld();
}

void FlockingBehaviorExampleApp::mouseDown( MouseEvent event ) {
}

void FlockingBehaviorExampleApp::update() {
	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);
}

void FlockingBehaviorExampleApp::draw() {
	gl::clear(Color(0,0,0));
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

void FlockingBehaviorExampleApp::createUserInterface() {
	mParams = ci::params::InterfaceGl::create("Flocking Behavior Example", vec2(250, 350));
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
	mParams->addText("Simulation Controls");
	mParams->addButton("Reset Simulation", [=]() {
		resetWorld();
	});
}

void FlockingBehaviorExampleApp::createWorld() {
	float worldSize = mUnits.getPixelsfromMeters(50);

	// create walls
	auto floor = createWall(worldSize, vec3(1, 1, 0), vec3(1, 0, 0), vec3(0, 1, 0));
	auto ceiling = createWall(worldSize, vec3(1, 1, 0), vec3(-1, 0, 0), vec3(0, 1, 0));
	auto front = createWall(worldSize, vec3(1, 0, 1), vec3(0, 1, 0), vec3(0, 0, 1));
	auto back = createWall(worldSize, vec3(1, 0, 1), vec3(0, -1, 0), vec3(0, 0, 1));
	auto left = createWall(worldSize, vec3(0, 1, 1), vec3(1, 0, 0), vec3(0, 0, 1));
	auto right = createWall(worldSize, vec3(0, 1, 1), vec3(-1, 0, 0), vec3(0, 0, 1));

	// create obstacles
	float maxObstacleSize = mUnits.getPixelsfromMeters(10);
	for (int i = 0; i < 5; i++) {
		auto obstacle = mEntities.create();
		vec3 size = maxObstacleSize * ci::randVec3();
		obstacle.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createBox(size, 0.0, 0.4f * worldSize * ci::randVec3()));
		obstacle.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCube(size), Color(0.0f, ci::randFloat(), ci::randFloat()));
		obstacle.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);
	}

	for (int i = 0; i < 15; i++) {
		auto boid = mEntities.create();
		float radius = mUnits.getPixelsfromMeters(1);
		boid.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createSphere(radius, 1.0, 0.4f * worldSize * ci::randVec3()));
		boid.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(0.8, 0, 1));
		boid.assign<sitara::ecs::LogicalLayer>(LayerNames::BOIDS);
	}

}

void FlockingBehaviorExampleApp::resetWorld() {
	float worldSize = mUnits.getPixelsfromMeters(50);

	entityx::ComponentHandle<sitara::ecs::LogicalLayer> layer;
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;

	for (auto entity : mEntities.entities_with_components(body, layer)) {
		if (layer->mLayerId == LayerNames::OBSTACLES) {
			body->resetBody(0.4f * worldSize * ci::randVec3());
		}
		else if (layer->mLayerId == LayerNames::BOIDS) {
			body->resetBody(0.4f * worldSize * ci::randVec3());
		}
	}
}

entityx::Entity FlockingBehaviorExampleApp::createWall(float size, vec3 dimensions, vec3 u, vec3 v) {
	auto wall = mEntities.create();
	wall.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createBox(size * dimensions, 0.0, 0.5f * size * glm::cross(u, v)));
	wall.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWirePlane(u, v, vec2(size, size), sitara::ecs::geometry::Quality::LOW), 
										Color(1.0, 1.0, 1.0));
	wall.assign<sitara::ecs::LogicalLayer>(LayerNames::WALLS);
	return wall;
}

CINDER_APP( FlockingBehaviorExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setFullScreen(true); })
