#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"
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
	std::vector<std::string> mBehaviorList = { "Seek", "Flee", "Arrive", "Wander", "Flock"};
	int mBehaviorSelect = 0;

	enum LayerNames {
		WALLS,
		OBSTACLES,
		BOIDS,
		TARGET
	};

	float mSeparationStrength = 100.0f;
	float mCohesionStrength = 100.0f;
	float mAlignmentStrength = 100.0f;

};

FlockingBehaviorExampleApp::FlockingBehaviorExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void FlockingBehaviorExampleApp::setup() {
	ci::Rand::randomize();
	ci::app::setFrameRate(60);

	sitara::ecs::configureSystems(mSystems);
	mSystems.add<sitara::ecs::BehaviorSystem>();
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	physics->setGhostCollisions(true);
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.configure();

	mCameraDistance = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(75.0f);
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
	entityx::ComponentHandle<sitara::ecs::Target> target;
	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<sitara::ecs::LogicalLayer> layer;

	for (auto entity : mEntities.entities_with_components(target, layer, transform)) {
		if (layer->mLayerId == LayerNames::TARGET) {
			target->setTargetPosition(ci::vec3(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(20.0) * std::cos(2.0 * M_PI * ci::app::getElapsedSeconds() / 8.0f),
				sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(20.0) * std::sin(2.0 * M_PI * ci::app::getElapsedSeconds() / 8.0f),
												0.0));
			transform->mPosition = target->getTargetPosition();
		}
		else if (layer->mLayerId == LayerNames::BOIDS) {
			if (mBehaviorSelect == 0) {
				mSystems.system<sitara::ecs::BehaviorSystem>()->seek(entity);
			}
			else if (mBehaviorSelect == 1) {
				mSystems.system<sitara::ecs::BehaviorSystem>()->flee(entity);
			}
			else if (mBehaviorSelect == 2) {
				mSystems.system<sitara::ecs::BehaviorSystem>()->arrive(entity);
			}
			else if (mBehaviorSelect == 3) {
				mSystems.system<sitara::ecs::BehaviorSystem>()->wander(entity);
			}
			else if (mBehaviorSelect == 4) {
				mSystems.system<sitara::ecs::BehaviorSystem>()->separate(entity, mEntities);
				mSystems.system<sitara::ecs::BehaviorSystem>()->cohere(entity, mEntities);
				mSystems.system<sitara::ecs::BehaviorSystem>()->align(entity, mEntities);
			}

			target->setTargetPosition(ci::vec3(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(20.0) * std::cos(2.0 * M_PI * ci::app::getElapsedSeconds() / 8.0f),
				sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(20.0) * std::sin(2.0 * M_PI * ci::app::getElapsedSeconds() / 8.0f),
				0.0));
		}
	}

	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void FlockingBehaviorExampleApp::draw() {
	gl::clear(Color(0,0,0));
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
	mParams->addParam("Select Behavior", mBehaviorList, &mBehaviorSelect);
	mParams->addParam("Separation Strength", &mSeparationStrength).min(0.0f).max(2000.0f).step(100.0f).updateFn([&]() {
		entityx::ComponentHandle<sitara::ecs::Separation> separation;
		for (auto entities : mEntities.entities_with_components(separation)) {
			separation->mWeight = mSeparationStrength;
		}
	});
	mParams->addParam("Cohesion Strength", &mCohesionStrength).min(0.0f).max(2000.0f).step(100.0f).updateFn([&]() {
		entityx::ComponentHandle<sitara::ecs::Cohesion> cohesion;
		for (auto entities : mEntities.entities_with_components(cohesion)) {
			cohesion->mWeight = mCohesionStrength;
		}
	});
	mParams->addParam("Alignment Strength", &mAlignmentStrength).min(0.0f).max(2000.0f).step(100.0f).updateFn([&]() {
		entityx::ComponentHandle<sitara::ecs::Alignment> alignment;
		for (auto entities : mEntities.entities_with_components(alignment)) {
			alignment->mWeight = mAlignmentStrength;
		}
	});
}

void FlockingBehaviorExampleApp::createWorld() {
	float worldSize = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(50);

	// create walls
	auto floor = createWall(worldSize, vec3(1, 1, 0), vec3(1, 0, 0), vec3(0, 1, 0));
	auto ceiling = createWall(worldSize, vec3(1, 1, 0), vec3(-1, 0, 0), vec3(0, 1, 0));
	auto front = createWall(worldSize, vec3(0, 1, 1), vec3(0, 1, 0), vec3(0, 0, 1));
	auto back = createWall(worldSize, vec3(0, 1, 1), vec3(0, -1, 0), vec3(0, 0, 1));
	auto left = createWall(worldSize, vec3(1, 0, 1), vec3(1, 0, 0), vec3(0, 0, 1));
	auto right = createWall(worldSize, vec3(1, 0, 1), vec3(-1, 0, 0), vec3(0, 0, 1));

	// create obstacles
	for (int i = 0; i < 10; i++) {
		auto obstacle = mEntities.create();
		float radius = ci::randFloat(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(5));
		obstacle.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createSphere(radius, 0.0,
																						0.5f * worldSize * ci::vec3(ci::randFloat(-1, 1), ci::randFloat(0.25, 0.75), ci::randFloat(-1, 1))));
		obstacle.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(0.0f, ci::randFloat(), ci::randFloat()));
		obstacle.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);
	}

	// create proximity detector
	auto proximity = mEntities.create();
	float edge = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(22);
	ci::vec3 proxPosition = 0.5f * worldSize * ci::vec3(0.0, -0.5, 0.0);
	auto ghostBody = proximity.assign<sitara::ecs::GhostBody>(sitara::ecs::GhostBody::createBox(ci::vec3(edge), proxPosition));
	ghostBody->addOnEnterGhostCollisionFn([&](entityx::ComponentHandle<sitara::ecs::RigidBody> body) {
		std::printf("Collision Began at %d\n", ci::app::getElapsedFrames());
		auto e = body.entity();
		auto geometry = e.component<sitara::ecs::Geometry>();
		geometry->setColor(ci::Color(1, 1, 1));
		});
	ghostBody->addOnEndGhostCollisionFn([&](entityx::ComponentHandle<sitara::ecs::RigidBody> body) {
		std::printf("Collision ended at %d\n", ci::app::getElapsedFrames());
		auto e = body.entity();
		auto geometry = e.component<sitara::ecs::Geometry>();
		geometry->setColor(ci::Color(0.8, 0.0, 1));
		});
	proximity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWireCube(ci::vec3(edge)), Color(1, 1, 1));
	proximity.assign<sitara::ecs::LogicalLayer>(LayerNames::OBSTACLES);

	// create boids
	for (int i = 0; i < 1; i++) {
		auto boid = mEntities.create();
		float radius = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(1);
		boid.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createSphere(radius, 1.0, 0.4f * worldSize * ci::randVec3()));
		boid.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(0.8, 0, 1));
		boid.assign<sitara::ecs::LogicalLayer>(LayerNames::BOIDS);
		boid.assign<sitara::ecs::Target>(ci::vec3(0), 250.0f, sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(3));
		boid.assign<sitara::ecs::NoiseField>(ci::vec4(ci::randVec3(), ci::randFloat()));
		boid.assign<sitara::ecs::Separation>(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(15), mSeparationStrength);
		boid.assign<sitara::ecs::Cohesion>(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(15), mCohesionStrength);
		boid.assign<sitara::ecs::Alignment>(sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(15), mAlignmentStrength);
	}

	auto target = mEntities.create();
	float targetSize = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(1);
	target.assign<sitara::ecs::Target>(ci::vec3(0), 250.0f, sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(50));
	target.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(targetSize), Color(1, 1, 0));
	target.assign<sitara::ecs::LogicalLayer>(LayerNames::TARGET);
	target.assign<sitara::ecs::Transform>();
}

void FlockingBehaviorExampleApp::resetWorld() {
	float worldSize = sitara::ecs::Units::getInstance(10.0).getPixelsFromMeters(50);

	entityx::ComponentHandle<sitara::ecs::LogicalLayer> layer;
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;

	for (auto entity : mEntities.entities_with_components(body, layer)) {
		/*
		if (layer->mLayerId == LayerNames::OBSTACLES) {
			mSystems.system<sitara::ecs::PhysicsSystem>()->resetBody(body, vec3(0.5 * worldSize * ci::randFloat(-0.5, 0.5), 0.5 * worldSize, 0.5 * worldSize * ci::randFloat(-0.5, 0.5)));
		}
		*/
		if (layer->mLayerId == LayerNames::BOIDS) {
			entity.component<sitara::ecs::Geometry>()->setColor(Color(0.8, 0, 1));
			mSystems.system<sitara::ecs::PhysicsSystem>()->resetBody(body, 0.4f * worldSize * ci::randVec3());
		}
	}
}

entityx::Entity FlockingBehaviorExampleApp::createWall(float size, ci::vec3 dimensions, ci::vec3 u, ci::vec3 v) {
	auto wall = mEntities.create();
	vec3 wSize = size * dimensions;
	vec3 position = 0.5f * size * glm::cross(u, v);
	wall.assign<sitara::ecs::RigidBody>(sitara::ecs::RigidBody::createBox(wSize, 0.0f, position));
	wall.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWirePlane(u, v, vec2(size, size), sitara::ecs::geometry::Quality::LOW),
										Color(1.0, 1.0, 1.0));
	wall.assign<sitara::ecs::LogicalLayer>(LayerNames::WALLS);
	return wall;
}

CINDER_APP( FlockingBehaviorExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setFullScreen(true); })
