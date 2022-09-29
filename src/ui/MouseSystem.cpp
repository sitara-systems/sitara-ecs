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
    auto point = ci::vec3(event.getPos(), 0.0f);

    std::function<void(sitara::ecs::TransformHandle, sitara::ecs::TransformHandle)> clickFn =
        [&, point](const sitara::ecs::TransformHandle parentHandle, sitara::ecs::TransformHandle childHandle) {
            if (childHandle->isShowing()) {
                sitara::ecs::Clickable2DHandle clickHandle = childHandle.entity().component<sitara::ecs::Clickable2D>();
                if (clickHandle.valid()) {
                    ci::Rectf rect = clickHandle->getBoundingBox();
                    auto world = childHandle->getWorldTransform();
                    rect.offset(world[3]);  // world[3] is the translation part of the matrix
                    if (rect.contains(point)) {
                        mSelectedEntity = childHandle.entity();
                        mDragData.mDragStartPosition = ci::vec3(event.getPos(), 0);
                        mDragData.mEntityStartPosition = childHandle->mPosition;
                        if (clickHandle->mOnDownFn != nullptr) {
                            clickHandle->mOnDownFn(mSelectedEntity);
                        }
                    }                
                }
            }
        };

    sitara::ecs::TransformHandle transform;
    mSelectedEntity.invalidate();
    auto transformSystem = mSystems.system<sitara::ecs::TransformSystem>();

    for (auto entity : mEntities.entities_with_components(transform)) {
        if (transform->isRoot() && transform->isShowing()) {
            transformSystem->descend(transform, clickFn);
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
