#include "ui/MouseSystem.h"

#include "geometry/Geometry.h"
#include "transform/Transform.h"
#include "ui/Clickable2D.h"
#include <functional>

using namespace cinder;
using namespace sitara::ecs;

void MouseSystem::configure(entityx::EventManager& events) {
    auto window = app::getWindow();

    mSignals.emplace_back(std::make_shared<ci::signals::ScopedConnection>(window->getSignalMouseDown().connect(std::bind(&MouseSystem::mouseDown, this, std::placeholders::_1))));
    mSignals.emplace_back(std::make_shared<ci::signals::ScopedConnection>(window->getSignalMouseDrag().connect(std::bind(&MouseSystem::mouseDrag, this, std::placeholders::_1))));
    mSignals.emplace_back(std::make_shared<ci::signals::ScopedConnection>(window->getSignalMouseMove().connect(std::bind(&MouseSystem::mouseMove, this, std::placeholders::_1))));
    mSignals.emplace_back(std::make_shared<ci::signals::ScopedConnection>(window->getSignalMouseUp().connect(std::bind(&MouseSystem::mouseUp, this, std::placeholders::_1))));
    mSignals.emplace_back(std::make_shared<ci::signals::ScopedConnection>(window->getSignalMouseWheel().connect(std::bind(&MouseSystem::mouseWheel, this, std::placeholders::_1))));
}

void MouseSystem::mouseDown(ci::app::MouseEvent& event) {
    entityx::ComponentHandle<Clickable2D> clickable;
    entityx::ComponentHandle<Transform> transform;
    mSelectedEntity.invalidate();

    auto point = ci::vec3(event.getPos(), 0.0f);

    for (auto e : mEntities.entities_with_components(transform, clickable)) {
        ci::Rectf rect = clickable->getBoundingBox();
        rect.offset(transform->mPosition);
        if (rect.contains(point)) {
            onDown(point, e);
        }
    }
}

void MouseSystem::mouseDrag(ci::app::MouseEvent& event) {
    if (mSelectedEntity) {
        entityx::ComponentHandle<Clickable2D> clickable;
        entityx::ComponentHandle<Transform> transform;
        mSelectedEntity.unpack(clickable, transform);

        transform->mPosition = mEntityStartPosition + (vec3(event.getPos(), 0.0f) - mDragStartPosition) * ci::vec3(1.0f);
    }
}

void MouseSystem::mouseMove(ci::app::MouseEvent& event) {}
void MouseSystem::mouseUp(ci::app::MouseEvent& event) {}
void MouseSystem::mouseWheel(ci::app::MouseEvent& event) {}

void MouseSystem::onDown(const ci::vec3& mousePosition, entityx::Entity selectedEntity) {
    mDragStartPosition = mousePosition;
    mSelectedEntity = selectedEntity;
    mEntityStartPosition = selectedEntity.component<sitara::ecs::Transform>()->mPosition;
}
