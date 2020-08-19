# sitara-ecs
Entity Component System for Cinder, used by Sitara Systems LLC

Heavily influenced by (and some code outright borrowed from!) Sosolimited's [Entity Component Samples](https://github.com/sosolimited/Entity-Component-Samples).

## Requirements
### cinder
Tested with v0.9.2

### EntityX
EntityX is included as a submodule.  You'll need to run CMake and configure the CMake files to copy files into the appropriate directories.  You do *not* need to build the project.

### Bullet3
Install Bullet3 using `vcpkg` using the instructions at [https://github.com/bulletphysics/bullet3](https://github.com/bulletphysics/bullet3)

## Usage

### Behavior System
- Autonomous behaviors to control the motion of entities

### Geometry System
- Drawing wireframe and solid shapes
- Coming Soon : Shader Support

### Logic System
- Logical Layers provide simple layer labels for objects
- Logical States and State Machine

### Physics System
- Bullet Physics for rigid body collisions
- Automatically adds Transforms to read position and orientation data from
- Coming Soon : Soft Body Physics

### Transform System
- World and Local Transforms with Parent/Child Relationships

### Utilities
- Unit Systems to help keep pixel-to-unit conversions consistent
- Simplex Noise
