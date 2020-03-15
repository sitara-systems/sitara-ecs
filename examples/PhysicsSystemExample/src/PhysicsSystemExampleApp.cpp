#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
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

	struct Box {
		Box(vec3 size, Color color) :
			mDimensions(size),
			mColor(color)
		{
			auto mesh = geom::Cube();
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
};

PhysicsSystemExampleApp::PhysicsSystemExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void PhysicsSystemExampleApp::setup() {
	auto physics = mSystems.add<sitara::ecs::PhysicsSystem>();
	mSystems.configure();

	physics->setGravity(vec3(0.0, -10.0, 0.0));

	auto ground = mEntities.create();
	vec3 position = vec3(0, -56, 0);
	ground.assign<sitara::ecs::RigidBody>(sitara::ecs::BOX, vec3(0, -56.0, 0), vec3(50, 50, 50));
	ground.assign<sitara::Box>(vec3(50, 50, 50), Color(1, 1, 1));

	for (int i = 0; i < 1; i++) {
		auto ps = vec3(0, 0, 0);
		auto ball = mEntities.create();
		ball.assign<sitara::ecs::RigidBody>(sitara::ecs::SPHERE, vec3(0, 0, 0), vec3(1, 1, 1), 1.0);
		ball.assign<sitara::Sphere>(1.0, Color(0.0, 1.0, 1.0));
	}
}

void PhysicsSystemExampleApp::mouseDown( MouseEvent event ) {
}

void PhysicsSystemExampleApp::update() {
	mSystems.update<sitara::ecs::PhysicsSystem>(1.0 / 60.0);

	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	for (auto entity : mEntities.entities_with_components(body)) {
		vec3 p = body->getPosition();
		std::printf("Body Position: %f %f %f\n", p.x, p.y, p.z);
	}
}

void PhysicsSystemExampleApp::draw() {
	entityx::ComponentHandle<sitara::ecs::RigidBody> body;
	entityx::ComponentHandle<sitara::Sphere> sphere;
	entityx::ComponentHandle<sitara::Box> box;

	for (auto entity : mEntities.entities_with_components(body, sphere)) {
		vec3 p = body->getPosition();
		sphere->mSphere->draw();
	}

	for (auto entity : mEntities.entities_with_components(body, box)) {
		vec3 p = body->getPosition();
		box->mBox->draw();
	}
}

CINDER_APP( PhysicsSystemExampleApp, RendererGl )
