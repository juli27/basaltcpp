# Roadmap
## v0.4.0
* First draft of the architecture
  * which sub projects: runtime, framework, platform launcher, ...
  * core systems (memory, resources, gfx backend, ...)
* Support creation of third party client apps
  * Easy setup of the project files
  * location of the client app source: in or out of tree
    * installation of the engine (CMake) ?

## v0.5.0
* OpenGL 1.0 renderer

## 0.6.0
* GLFW backend ([#11](https://github.com/juli27/basaltcpp/issues/11))
  * Windows + OpenGL renderer
  * Linux + OpenGL renderer
* Linux x86-64 support
  * CMake support
  * GCC and libstdc++

## Unscheduled
* turn /permissive- back on
* Model loading with automatic Mesh and Material management
* Direct3D 9 programmable pipeline renderer
* upgrade Direct3D 9 renderer to Direct3D 9Ex
* upgrade OpenGL renderer to OpenGL 1.1+ (up to OpenGL 2.1)
* GLES 2.0 renderer
* Linux + Wayland + EGL support
* Android support
* Direct3D 10 renderer
* Direct3D 11 renderer
* Direct3D 12 renderer
* Vulkan 1.0 renderer
* audio support
* controller support
* animation
* physics
* editor
* unified resource management
* entities
  * prototypes
  * shared components
