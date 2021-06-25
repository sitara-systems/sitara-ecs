#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CinderImGui.h"
#include "Ecs.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ParticleSystemExampleApp : public App {
public:
	ParticleSystemExampleApp();
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDrag(MouseEvent event) override;
	void keyDown(KeyEvent event) override;

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;

	enum Layers {
		MOUSE,
		OBJECT,
		SPRING
	};

	ci::CameraPersp mCamera;
	float mCameraDistance;
	vec3 mEye;
	vec3 mUp;
	vec3 mCenter;

	float mSpringConstant = 0.3f;
	float mDampening = 0.1f;
	float mAttractorStrength = 96000.0f;

	bool mShowForces = false;
};

ParticleSystemExampleApp::ParticleSystemExampleApp() :
	mEntities(mEvents),
	mSystems(mEntities, mEvents) {
}

void ParticleSystemExampleApp::setup() {
	sitara::ecs::configureSystems(mSystems);
	mSystems.add<sitara::ecs::ParticleSystem>();
	mSystems.add<sitara::ecs::TransformSystem>();
	mSystems.configure();

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			float x = i * 1080.0 / 14.0 + 0.25f * 960.0;
			float y = j * 1080.0 / 14.0 + 0.25f * 960.0;
			auto pos = vec3(x, y, 0.0f);
			auto ball = mEntities.create();
			float radius = 10.0;
			auto particleHandle = ball.assign<sitara::ecs::Particle>(0.5f, pos);
			ball.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(ci::Rand::randFloat(), ci::Rand::randFloat(), ci::Rand::randFloat()));
			ball.assign<sitara::ecs::LogicalLayer>(Layers::OBJECT);

			auto spring = mEntities.create();
			spring.assign<sitara::ecs::Transform>(pos);
			spring.assign<sitara::ecs::Spring>(particleHandle, pos, mSpringConstant, mDampening, 0.1f);
			spring.assign<sitara::ecs::LogicalLayer>(Layers::SPRING);
			spring.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(5.0f), Color(0.5, 0.5, 0.5));
		}
	}

	auto attractor = mEntities.create();
	float radius = 25.0;
	auto force = attractor.assign<sitara::ecs::Attractor>(ci::vec3(0), ci::vec3(mAttractorStrength));
	force->turnOff();
	attractor.assign<sitara::ecs::Geometry>(sitara::ecs::geometry::createSphere(radius), Color(1, 1, 1));
	attractor.assign<sitara::ecs::LogicalLayer>(Layers::MOUSE);

	// setup camera orientation vectors
	float center = 1080.0f / 2.0f;
	mCameraDistance = 1200;// 692;
	mEye = vec3(center, center, mCameraDistance);
	mCenter = vec3(center, center, 0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);
	// setup the camera frustum

	mCamera.setPerspective(60.0f, 1.0f, 5.0f, 10000.0f);
	mCamera.lookAt(mEye, mCenter, mUp);

	ImGui::Initialize();
}

void ParticleSystemExampleApp::update() {
	ImGui::Begin("Parameters");
	ImGui::SliderFloat("Spring Constant", &mSpringConstant, 0.001, 1.0f);
	ImGui::SliderFloat("Dampening", &mDampening, 0.001, 1.0f);
	ImGui::SliderFloat("Attractor Strength", &mAttractorStrength, 1.0f, 50000.0f);
	ImGui::End();

	entityx::ComponentHandle<sitara::ecs::Spring> spring;
	entityx::ComponentHandle<sitara::ecs::Attractor> attractor;

	for (auto entity : mEntities.entities_with_components(spring)) {
		spring->setDamping(mDampening);
		spring->setSpringConstant(mSpringConstant);
	}

	for (auto entity : mEntities.entities_with_components(attractor)) {
		attractor->setStrength(ci::vec3(mAttractorStrength));
	}

	mSystems.update<sitara::ecs::ParticleSystem>(1.0 / 60.0);
	mSystems.update<sitara::ecs::TransformSystem>(1.0 / 60.0);
}

void ParticleSystemExampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::setMatrices(mCamera);

	gl::enableDepthRead();
	gl::enableDepthWrite();

	entityx::ComponentHandle<sitara::ecs::Transform> transform;
	entityx::ComponentHandle<sitara::ecs::Geometry> geom;

	/*
	* Draw everything with a Geometry component -- springs and particles
	*/
	for (auto entity : mEntities.entities_with_components(transform, geom)) {
		gl::pushModelMatrix();
		gl::setModelMatrix(transform->getWorldTransform());
		geom->draw();
		ci::Color(0, 0, 1);
		gl::popModelMatrix();
	}

	/*
	* Draw Spring Forces
	*/
	if (mShowForces) {
		entityx::ComponentHandle<sitara::ecs::Spring> spring;
		for (auto entity : mEntities.entities_with_components(spring, transform)) {
			std::cout << ci::app::getElapsedSeconds() << " | Drawing Spring " << std::endl;
			gl::pushModelMatrix();
			gl::setModelMatrix(transform->getWorldTransform());

			gl::color(1, 0, 0);
			ci::vec3 springForce = spring->getSpringForce();
			glm::normalize(springForce);
			gl::drawLine(ci::vec2(0), 10.0f * ci::vec2(springForce));

			gl::color(0, 1, 0);
			ci::vec3 dampingForce = spring->getDampingForce();
			glm::normalize(dampingForce);
			gl::drawLine(ci::vec2(0), 10.0f * ci::vec2(dampingForce));

			gl::popModelMatrix();
		}
	}

	gl::disableDepthWrite();
	gl::disableDepthRead();
}

void ParticleSystemExampleApp::mouseDrag(MouseEvent event) {
	entityx::ComponentHandle<sitara::ecs::Attractor> attractor;
	for (auto entity : mEntities.entities_with_components(attractor)) {
		attractor->setPosition(ci::vec3(event.getX(), ci::app::getWindowHeight() - event.getY(), 0));
	}
}

void ParticleSystemExampleApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'f') {
		entityx::ComponentHandle<sitara::ecs::Attractor> attractor;
		for (auto entity : mEntities.entities_with_components(attractor)) {
			if (attractor->IsOn()) {
				attractor->turnOff();
				std::cout << "Turning Attractor Off." << std::endl;
			}
			else {
				attractor->turnOn();
				std::cout << "Turning Attractor On." << std::endl;
			}
		}
	}
	else if (event.getChar() == 'd') {
		mShowForces = !mShowForces;
	}
}

CINDER_APP( ParticleSystemExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {
	settings->setTitle("Particle System Test");
	settings->setWindowSize(1080, 1080);
	settings->setConsoleWindowEnabled();
	settings->setHighDensityDisplayEnabled(false);
});
