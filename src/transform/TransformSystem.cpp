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
	sitara::ecs::TransformHandle transformHandle;

	for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        if (transformHandle->isRoot() && transformHandle->isShowing()) {
            transformHandle->updateWorldTransform(mat4(1));
            descend(transformHandle, [](const sitara::ecs::TransformHandle parent,
                                        sitara::ecs::TransformHandle child) {
                child->updateWorldTransform(parent->getWorldTransform());
            });

            if (mDepthSortEnabled) {
                transformHandle->sortChildrenByDepth();
            }
		}
	}
}

sitara::ecs::TransformHandle TransformSystem::attachChild(entityx::Entity parent, entityx::Entity child) {
    sitara::ecs::TransformHandle parentHandle;
	if (parent.has_component<Transform>()) {
        parentHandle = parent.component<Transform>();
    } else {
        parentHandle = parent.assign<Transform>();
    }

	sitara::ecs::TransformHandle childHandle;
	if (child.has_component<Transform>()) {
		childHandle = child.component<Transform>();
	}
	else {
		childHandle = child.assign<Transform>();
	}

	attachChild(parentHandle, childHandle);
	return childHandle;
}

void TransformSystem::attachChild(sitara::ecs::TransformHandle parentHandle, sitara::ecs::TransformHandle childHandle) {
	if (childHandle.get() != parentHandle.get()) {
		removeFromParent(childHandle);
		childHandle->setParent(parentHandle);
        childHandle->updateLabelPath();
		parentHandle->addChild(childHandle);
	}
}

void TransformSystem::removeFromParent(sitara::ecs::TransformHandle childHandle) {
	if (childHandle->getParent()) {
		childHandle->getParent()->removeChild(childHandle);
        childHandle->updateLabelPath();
	}
}

void TransformSystem::applyToRootNodes(entityx::EntityManager& entities,
    const std::function<void(sitara::ecs::TransformHandle, sitara::ecs::TransformHandle)>& function) {
    sitara::ecs::TransformHandle transformHandle;
    for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        if(transformHandle->isRoot()) {
            function(sitara::ecs::TransformHandle(), transformHandle);
        }
    }
}

sitara::ecs::TransformHandle TransformSystem::getNodeByLabel(entityx::EntityManager& entities, std::string label) {
    /*
    * 
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
    std::function<sitara::ecs::TransformHandle(std::vector<sitara::ecs::TransformHandle>,
                                                      std::queue<std::string>)>
        searchFn = [&](std::vector<sitara::ecs::TransformHandle> children, std::queue<std::string> labels) {
            std::string searchLabel = labels.front();
            auto it = std::find_if(children.begin(), children.end(), [&](sitara::ecs::TransformHandle transform) {
                std::string testLabel = transform->getLabel();
                return (testLabel == searchLabel);
            });
            labels.pop();
            if (it != children.end()) {
                if (labels.size() == 0) {
                    // no more labels, we found it!
                    return *it;
                } else {
                    if ((*it)->getChildren().size()) {
                        // more labels, more children to search
                        return searchFn((*it)->getChildren(), labels);
                    } else {
                        // more labels, but no more children -- dead end
                        CI_LOG_W("No transform component found with label "
                                 << searchLabel << "; partial match found but no exact match.");
                        return sitara::ecs::TransformHandle();
                    }
                }
            } else {
                // did not find a child at this level with the correct label
                CI_LOG_W("No node found in tree with label " << searchLabel);
                return sitara::ecs::TransformHandle();
            }
        };

    // search through hierarchy, starting with roots
    std::string searchLabel = labelQueue.front();
    sitara::ecs::TransformHandle transformHandle;
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
                    CI_LOG_W("No transform component found with label "
                                << searchLabel << "; partial match found but no exact match.");
                    return sitara::ecs::TransformHandle();
                }
            }
        }
    }
    CI_LOG_W("No transform component found with label " << searchLabel);
    return sitara::ecs::TransformHandle();
}

//! Ascends up the tree and applies a function to each node and its parent
//! The root nodes don't have any parent, so be sure to check if the parentHandle is valid!
//! Parent handle is only provided as a reference (hence the const)
void TransformSystem::ascend(sitara::ecs::TransformHandle childHandle,
                             const std::function<void(const sitara::ecs::TransformHandle,
                                                      sitara::ecs::TransformHandle)>& function) {
    sitara::ecs::TransformHandle parent = childHandle->getParent();
    if (parent.valid()) {
        function(parent, childHandle);
        ascend(parent, function);
    }
}

//! Descends down the tree and applies a function to each node and its parent
//! Parent handle is only provided as a reference (hence the const)
void TransformSystem::descend(sitara::ecs::TransformHandle rootHandle, const std::function<void(const sitara::ecs::TransformHandle, sitara::ecs::TransformHandle)>& function) {
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

std::vector<std::pair<std::string, bool>> TransformSystem::getLabelTree(entityx::EntityManager& entities) {
    std::vector<std::pair<std::string, bool>> labelTree;
    sitara::ecs::TransformHandle transformHandle;

    for (entityx::Entity e : entities.entities_with_components(transformHandle)) {
        // start at root nodes
        if (transformHandle->isRoot()) {
            transformHandle->updateLabelPath();
            labelTree.push_back(
                std::make_pair<std::string, bool>(transformHandle->getLabelPath(), transformHandle->isShowing()));
            descend(transformHandle,
                    [&](const sitara::ecs::TransformHandle parent, sitara::ecs::TransformHandle child) {
                        child->updateLabelPath();
                        labelTree.push_back(std::make_pair<std::string, bool>(child->getLabelPath(),
                                                                              child->isShowing()));
            });
        }
    }

    std::function<bool(const std::pair<std::string, bool>, const std::pair<std::string, bool>)> compareFn =
        [&](const std::pair<std::string, bool> a, const std::pair<std::string, bool> b) {
            return a.first < b.first;
    };

    std::sort(labelTree.begin(), labelTree.end(), compareFn);
    return labelTree;
}
