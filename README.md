# sitara-ecs

Entity Component System for Cinder, used by Sitara Systems LLC

Heavily influenced by (and some code outright borrowed from!) Sosolimited's [Entity Component Samples](https://github.com/sosolimited/Entity-Component-Samples).

## Requirements

This library uses:

-   NVIDIA PhysX
-   entityx

The included `vcpkg.json` should automatically download and build the dependencies for all examples in here.  This block also requires cinder to be in an adjacent folder for the cinder dependencies to be found automatically:

    parentFolder/
        sitara-ecs/
        cinder/

Use the included `vcpkg.json` as a jumping off point for adding this block into other projects!

Some modules require other `sitara-` libraries.  These are activated by including a `#define` statement and having the required submodule included in the project.

These optional features include:

| Features         | Module     | Preprocessor Statement  | Required Submodule |
| ---------------- | ---------- | ----------------------- | ------------------ |
| Font Rendering   | TextSystem | `#define USING_SDFTEXT` | sitara-sdftext     |
| 3d Model Support | Geometry   | `#define USING_ASSIMP`  | sitara-assimp      |

## Current Features

### Behavior System

-   Autonomous behaviors to control the motion of entities

### Geometry System

-   Drawing wireframe and solid shapes
-   Coming Soon : Shader Support
-   Coming Soon : ASSIMP Support for 3d objects (requires `sitara-assimp`)

### Logic System

-   Logical Layers provide simple layer labels for objects
-   Logical States and State Machine

### Physics System

-   NVIDIA PhysX for rigid body collisions
-   Automatically adds Transforms to store position and orientation data
-   Coming Soon : Soft Body Physics

### Transform System

-   World and Local Transforms with Parent/Child Relationships

### Utilities

-   Unit Systems to help keep pixel-to-unit conversions consistent
-   Simplex Noise

## To Do

-   Physics
    -   Cooking
    -   Multithreading and thread locking
    -   GPU
    -   Shared Shapes (alternative to Exclusive Shapes)
    -   Switch messages to Cinder Logging
