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
	int mElasticSelect = 0;
	std::vector<std::string> mFrictionEnums = { "On", "Off" };
	int mFrictionSelect = 0;
	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;

	enum LayerNames {
		GROUND,
		BALL
	};

	int mBallMaterialId;
};

PhysicsSystemExampleApp::PhysicsSystemExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void PhysicsSystemExampleApp::setup() {
	ci::app::setFrameRate(60);
	sitara::ecs::configureSystems(mSystems);
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.add<sitara::ecs::TransformSystem>();
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
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void PhysicsSystemExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
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
		entityx::ComponentHandle<sitara::ecs::LogicalLayer> layer;
		entityx::ComponentHandle<sitara::ecs::Geometry> geom;
		for (auto entity : mEntities.entities_with_components(geom, layer)) {
			if (layer->mLayerId == LayerNames::BALL) {
				if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::SPHERE) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWireSphere(5.0), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
				else if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::WIRE_SPHERE) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(5.0), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
				if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::BOX) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWireBox(ci::vec3(5.0)), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
				else if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::WIRE_BOX) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(ci::vec3(5.0)), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
				if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::CAPSULE) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createWireCapsule(5.0, 10), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
				else if (geom->getPrimitive() == sitara::ecs::geometry::Primitive::WIRE_CAPSULE) {
					entity.remove<sitara::ecs::Geometry>();
					entity.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCapsule(5.0, 10), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
				}
			}
		}
	});

	mParams->addText("Simulation Controls");
	mParams->addButton("Reset Simulation", [=]() {
		resetWorld();
	});
	mParams->addParam("Set Elasticity", mElasticEnums, &mElasticSelect).updateFn([=]() {
		physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(mBallMaterialId);

		if (mElasticSelect == 0) {
			ballMaterial->setRestitution(1.0f);
		}
		else if (mElasticSelect == 1) {
			ballMaterial->setRestitution(0.0f);
		}
	});
	mParams->addParam("Set Friction", mFrictionEnums, &mFrictionSelect).updateFn([=]() {
		physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(mBallMaterialId);

		if (mFrictionSelect == 0) {
			ballMaterial->setStaticFriction(1.0f);
			ballMaterial->setDynamicFriction(1.0f);
		}
		else if (mFrictionSelect == 1) {
			ballMaterial->setStaticFriction(0.0f);
			ballMaterial->setDynamicFriction(0.0f);
		}
	});
}

void PhysicsSystemExampleApp::createWorld() {

	{
		int groundMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(0, 0, 0);
		physx::PxMaterial* groundMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(groundMaterialId);

		auto ground = mEntities.create();
		vec3 halfSize = vec3(250, 1, 250);
		vec3 position = vec3(0, 0, 0);

		auto body = ground.assign<sitara::ecs::StaticBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createStaticBody(position));
		body->attachBox(halfSize, groundMaterial);
		ground.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(halfSize), Color(1.0, 1.0, 1.0));
		ground.assign<sitara::ecs::LogicalLayer>(LayerNames::GROUND);
	}

	mBallMaterialId = mSystems.system<sitara::ecs::PhysicsSystem>()->registerMaterial(1.0f, 1.0f, 1.0f);
	physx::PxMaterial* ballMaterial = mSystems.system<sitara::ecs::PhysicsSystem>()->getMaterial(mBallMaterialId);

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150));
		auto ball = mEntities.create();
		float radius = 5.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachSphere(radius, ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::BALL);
	}

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150));
		auto ball = mEntities.create();
		float halfEdge = 5.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachBox(ci::vec3(halfEdge), ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createBox(ci::vec3(halfEdge)), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::BALL);
	}

	for (int i = 0; i < 50; i++) {
		auto pos = vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150));
		auto ball = mEntities.create();
		float radius = 5.0;
		float height = 10.0;
		auto body = ball.assign<sitara::ecs::DynamicBody>(mSystems.system<sitara::ecs::PhysicsSystem>()->createDynamicBody(pos));
		body->attachCapsule(radius, height, ballMaterial);
		ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createCapsule(radius, height), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
		ball.assign<sitara::ecs::LogicalLayer>(LayerNames::BALL);
	}

}

void PhysicsSystemExampleApp::resetWorld() {
	entityx::ComponentHandle<sitara::ecs::LogicalLayer> layer;
	entityx::ComponentHandle<sitara::ecs::DynamicBody> body;

	for (auto entity : mEntities.entities_with_components(body, layer)) {
		if (layer->mLayerId == LayerNames::BALL) {
			body->resetBody(ci::vec3(ci::Rand::randFloat(-150, 150), ci::Rand::randFloat(50, 150), ci::Rand::randFloat(-150, 150)));
		}
	}
}

CINDER_APP( PhysicsSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setWindowSize(1280, 720); })
