#include "ui/MouseSystem.h"

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
        if (transform->isShowing()) {
            ci::Rectf rect = clickable->getBoundingBox();
            auto world = transform->getWorldTransform();
            rect.offset(world[3]);  // world[3] is the translation part of the matrix
            if (rect.contains(point)) {
                mSelectedEntity = e;
                mDragData.mDragStartPosition = ci::vec3(event.getPos(), 0);
                mDragData.mEntityStartPosition = transform->mPosition;
                if (clickable->mOnDownFn != nullptr) {
                    clickable->mOnDownFn(e);
                }
            }        
        }
    }
}

void MouseSystem::mouseDrag(ci::app::MouseEvent& event) {
    if (mSelectedEntity) {
        entityx::ComponentHandle<Clickable2D> clickable;
        entityx::ComponentHandle<Transform> transform;
        mSelectedEntity.unpack(clickable, transform);

        if (clickable->mOnDragFn != nullptr) {
            // should use DragData struct
            clickable->mOnDragFn(mSelectedEntity);
        }
    }
}

void MouseSystem::mouseMove(ci::app::MouseEvent& event) {
    if (mSelectedEntity) {
        entityx::ComponentHandle<Clickable2D> clickable;
        entityx::ComponentHandle<Transform> transform;
        mSelectedEntity.unpack(clickable, transform);

        if (clickable->mOnMoveFn != nullptr) {
            // should use DragData struct
            clickable->mOnMoveFn(mSelectedEntity);
        }
    }
}

void MouseSystem::mouseUp(ci::app::MouseEvent& event) {
    if (mSelectedEntity) {
        entityx::ComponentHandle<Clickable2D> clickable;
        entityx::ComponentHandle<Transform> transform;
        mSelectedEntity.unpack(clickable, transform);

        if (clickable->mOnUpFn != nullptr) {
            // should use DragData struct
            clickable->mOnUpFn(mSelectedEntity);
        }
    }
}

void MouseSystem::mouseWheel(ci::app::MouseEvent& event) {}
