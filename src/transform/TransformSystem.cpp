#include <queue>
#include "cinder/Log.h"
#include "transform/TransformSystem.h"

using namespace cinder;
using namespace sitara::ecs;

TransformSystem::TransformSystem() : mDepthSortEnabled(false) {};

void TransformSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events) {
	events.subscribe<entityx::ComponentRemovedEvent<Transform>>(*this);
}

void TransformSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;

	for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        if (transformHandle->isRoot() && transformHandle->isShowing()) {
            transformHandle->updateWorldTransform(mat4(1));
            descend(transformHandle, [](const entityx::ComponentHandle<Transform> parent,
                                        entityx::ComponentHandle<Transform> child) {
                child->updateWorldTransform(parent->getWorldTransform());
            });
            if (mDepthSortEnabled) {
                transformHandle->sortChildrenByDepth();
            }
		}
	}
}

entityx::ComponentHandle<Transform> TransformSystem::attachChild(entityx::Entity parent, entityx::Entity child) {
    entityx::ComponentHandle<Transform> parentHandle;
	if (parent.has_component<Transform>()) {
        parentHandle = parent.component<Transform>();
    } else {
        parentHandle = parent.assign<Transform>();
    }

	entityx::ComponentHandle<Transform> childHandle;
	if (child.has_component<Transform>()) {
		childHandle = child.component<Transform>();
	}
	else {
		childHandle = child.assign<Transform>();
	}

	attachChild(parentHandle, childHandle);
	return childHandle;
}

void TransformSystem::attachChild(entityx::ComponentHandle<Transform> parentHandle, entityx::ComponentHandle<Transform> childHandle) {
	if (childHandle.get() != parentHandle.get()) {
		removeFromParent(childHandle);
		childHandle->setParent(parentHandle);
		parentHandle->addChild(childHandle);
	}
}

void TransformSystem::removeFromParent(entityx::ComponentHandle<Transform> childHandle) {
	if (childHandle->getParent()) {
		childHandle->getParent()->removeChild(childHandle);
	}
}

void TransformSystem::applyToRootNodes(entityx::EntityManager& entities,
    const std::function<void(entityx::ComponentHandle<Transform>, entityx::ComponentHandle<Transform>)>& function) {
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;
    for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        if(transformHandle->isRoot()) {
            function(entityx::ComponentHandle<sitara::ecs::Transform>(), transformHandle);
        }
    }
}

entityx::ComponentHandle<sitara::ecs::Transform> TransformSystem::getNodeByLabel(entityx::EntityManager& entities, std::string label) {
    /*
     * Traverse the transform hierarchy and look for a node with matching label.  Labels use an OSC-style path delimited
     * by "/". e.g. "root/layer1/child
     * Leading and trailing backslashes are ignored: '/home', 'home/', '/home/', and 'home' are all equivalent.
     */

    // breakdown the string into a std::queue of labels
    std::queue<std::string> labelQueue;
    size_t delimiterPosition = 0;
    std::string sublabel;

    if (label[0] == '/') {
        label.erase(0);
    }

    if (label.back() == '/') {
        label.erase(label.size() - 1);
    }

    while ((delimiterPosition = label.find("/")) != std::string::npos) {
        sublabel = label.substr(0, delimiterPosition);
        labelQueue.push(sublabel);
        label.erase(0, delimiterPosition + 1);
    }
    labelQueue.push(label);

    // recursive search function
    std::function<entityx::ComponentHandle<Transform>(std::vector<entityx::ComponentHandle<Transform>>,
                                                      std::queue<std::string>)>
        searchFn = [&](std::vector<entityx::ComponentHandle<Transform>> children, std::queue<std::string> labels) {
            std::string searchLabel = labels.front();
            auto it = std::find_if(children.begin(), children.end(), [&](entityx::ComponentHandle<Transform> transform) {
                std::string testLabel = transform->getLabel();
                return (testLabel == searchLabel);
            });
            labels.pop();
            if (it != children.end()) {
                if ((*it)->getChildren().size()) {
                    return searchFn((*it)->getChildren(), labels);
                }
            } else {
                CI_LOG_W("No node found in tree with label " << searchLabel);
                return entityx::ComponentHandle<Transform>();
            }
        };

    // search through hierarchy, starting with roots
    std::string searchLabel = labelQueue.front();
    entityx::ComponentHandle<sitara::ecs::Transform> transformHandle;
    for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        if (transformHandle->isRoot() && transformHandle->getLabel() == searchLabel) {
            labelQueue.pop();
            if (labelQueue.size() == 0) {
                // no more labels, we just want the root
                return transformHandle;
            } else {
                if (transformHandle->getChildren().size()) {
                    // more labels, more children to search
                    return searchFn(transformHandle->getChildren(), labelQueue);                 
                } else {
                    // more labels, but no more children -- dead end
                    CI_LOG_W("No transform component found with label " << searchLabel << "; partial match found but no exact match.");
                    return entityx::ComponentHandle<Transform>();
                }
            }
        } else {
            CI_LOG_W("No transform component found with label " << searchLabel);
            return entityx::ComponentHandle<Transform>();
        }
    }
}

//! Ascends up the tree and applies a function to each node and its parent
//! The root nodes don't have any parent, so be sure to check if the parentHandle is valid!
//! Parent handle is only provided as a reference (hence the const)
void TransformSystem::ascend(entityx::ComponentHandle<Transform> childHandle,
                             const std::function<void(const entityx::ComponentHandle<Transform>,
                                                      entityx::ComponentHandle<Transform>)>& function) {
    entityx::ComponentHandle<Transform> parent = childHandle->getParent();
    if (parent.valid()) {
        function(parent, childHandle);
        ascend(parent, function);
    }
}

//! Descends down the tree and applies a function to each node and its parent
//! Parent handle is only provided as a reference (hence the const)
void TransformSystem::descend(entityx::ComponentHandle<Transform> rootHandle, const std::function<void(const entityx::ComponentHandle<Transform>, entityx::ComponentHandle<Transform>)>& function) {
	for (auto& child : rootHandle->getChildren()) {
		function(rootHandle, child);
		descend(child, function);
	}
}

void TransformSystem::receive(const entityx::ComponentRemovedEvent<Transform>& event) {
	removeFromParent(event.component);
}

void TransformSystem::enableDepthSort(bool enabled) {
    mDepthSortEnabled = enabled;
}