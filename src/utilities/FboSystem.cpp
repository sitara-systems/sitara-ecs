#pragma once

#include "utilities/FboSystem.h"

using namespace sitara::ecs;

FboSystem::FboSystem() {
}

void FboSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
}

void FboSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) {
    entities.each<Fbo>([dt](entityx::Entity entity, sitara::ecs::Fbo& fbo) {
        fbo.bindFbo();
        fbo.mDrawFn(fbo, dt);
        fbo.unbindFbo();
    });
}
