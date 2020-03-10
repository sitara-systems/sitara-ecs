#pragma once

#include "entityx/Entity.h"
#include "cinder/Vector.h"

namespace sitara {
  namespace ecs {
    struct PhysicsBody {

    PhysicsBody() = default;
    PhysicsBody(const ci::vec3 &position, float drag = 0.1f)
    : mPosition(position),
      mPreviousPosition(position),
      mDrag(drag)
    {}

    void applyForce(const ci::vec3 &amount) {
      mAcceleration += amount;
    }

    void placeBody(const ci::vec3 &pos) {
      mPosition = pos;
      mPreviousPosition = pos;
    }

    ci::vec3 getVelocity() const {
      return mPosition - mPreviousPosition;
    }

	void bounce() {
		mAcceleration *= -1.0;
	}

    ci::vec3 mPosition;
    ci::vec3 mPreviousPosition;
    ci::vec3 mAcceleration;
    float mDrag = 0.1f;
    };
  }
}
