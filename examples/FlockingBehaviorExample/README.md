# FlockingBehaviorExample

Purple objects are boids with selectable behaviors from the drop-down menu.

Blue-Green objects are rigid body obstacles that must be avoided by the boids.

The white cube in the bottom-half of the world is a `GhostBody`; it detects collision but does not respond in the physics engine.  This can be used for proximity detection and zone-based behaviors, as shown with the `onEnter` and `onExit` callbacks.
