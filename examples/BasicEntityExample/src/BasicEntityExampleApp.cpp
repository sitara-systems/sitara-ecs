#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"

#include "entityx/config.h"
#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/quick.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace components {
	struct Position {
		Position() {
			mPosition = vec3(0, 0, 0);
		}

		Position(const ci::vec3 &position) {
			mPosition = position;
		}

		ci::vec3 mPosition;
	};

	struct Sphere {
		Sphere(float diameter) {
			auto mesh = geom::Sphere().radius(diameter / 2.0f).subdivisions(16);
			auto color = geom::Constant(geom::COLOR, Color(CM_HSV, 0.0f, 0.0f, 1.0f));
			auto lambert = gl::ShaderDef().lambert().color();
			auto shader = gl::getStockShader(lambert);
			mSphere = gl::Batch::create(mesh >> color, shader);
		}

		gl::BatchRef mSphere;
	};

	struct Orbital {
		Orbital(vec3 center, vec3 position) {
			mDiameter = glm::distance(vec2(0, 0), vec2(position.x, position.y));
			mTheta = ci::math<float>::atan2(position.y, position.x);
			mAngularVelocity = 0.05 * ci::randFloat();
		}

		float mDiameter;
		float mTheta;
		float mAngularVelocity;
	};
}



class BasicEntityExampleApp : public App {
  public:
	  BasicEntityExampleApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void createOrbital(MouseEvent event);

	entityx::EntityManager mEntities;
	entityx::EventManager mEvents;
	entityx::SystemManager mSystems;
};

BasicEntityExampleApp::BasicEntityExampleApp()
	: mEntities(mEvents),
	mSystems(mEntities, mEvents)
{}

void BasicEntityExampleApp::setup() {
}

void BasicEntityExampleApp::mouseDown( MouseEvent event ) {
	createOrbital(event);
}

void BasicEntityExampleApp::update() {
	entityx::ComponentHandle<components::Orbital> orbitalHandle;
	entityx::ComponentHandle<components::Position> positionHandle;

	/*
	In a more advanced ECS this would be a system!  But we're starting simple so this just has entities and components.
	*/
	for (auto e : mEntities.entities_with_components(positionHandle)) {
		entityx::ComponentHandle<components::Orbital> orbital = e.component<components::Orbital>();
		orbital->mTheta += orbital->mAngularVelocity;
		
		e.component<components::Position>()->mPosition.x = 0.5 * orbital->mDiameter * ci::math<float>::cos(orbital->mTheta);
		e.component<components::Position>()->mPosition.y = 0.5 * orbital->mDiameter * ci::math<float>::sin(orbital->mTheta);
	}

}

void BasicEntityExampleApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );

	gl::enableDepthWrite();
	gl::enableDepthRead();

	gl::ScopedMatrices scopeMatrices;
	{
		vec3 cameraTarget;
		cameraTarget = vec3(0.0f, 0.0f, 0.0f);

		CameraPersp camera(ci::app::getWindowWidth(), ci::app::getWindowHeight(), 60.0f);
		camera.setPerspective(60, ci::app::getWindowAspectRatio(), 1.0f, 10000.0f);
		camera.lookAt(normalize(vec3(0, 0, 1)) * 1000.0f, cameraTarget);

		gl::setMatrices(camera);

		entityx::ComponentHandle<components::Position> positionHandle;
		entityx::ComponentHandle<components::Sphere> sphereHandle;

		for (auto e : mEntities.entities_with_components(positionHandle, sphereHandle)) {
			ci::gl::pushMatrices();
			gl::ScopedGlslProg scopedGlslProg(gl::context()->getStockShader(gl::ShaderDef().color()));
			gl::translate(positionHandle->mPosition);
			sphereHandle->mSphere->draw();
			ci::gl::popMatrices();
		}

		gl::disableDepthRead();
		gl::disableDepthWrite();
	}

}

void BasicEntityExampleApp::createOrbital(MouseEvent event) {
	// Create an entity that tracks the mouse.
	auto new_entity = mEntities.create();
	new_entity.assign<components::Position>(750.0f * ci::randVec3());
	new_entity.assign<components::Sphere>(50.0f);

	entityx::ComponentHandle<components::Position> pos = new_entity.component<components::Position>();

	new_entity.assign<components::Orbital>(vec3(0,0,0), pos->mPosition);

	vec3 p = new_entity.component<components::Position>()->mPosition;
}

CINDER_APP( BasicEntityExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); settings->setFullScreen(true); })