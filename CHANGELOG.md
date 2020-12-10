# Changelog
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.4.0] - *Unreleased*
### Added
- Colors
- Engine object to query and configure runtime behaviour
  - Engine.mouseCursor to get or set the current mouse cursor
  - load resources
- **/types.h includes to prelude.h
- Dear ImGui clipboard support
- Dear ImGui IME support
- Gfx: `DrawTarget`
- Gfx: `Drawable`
  - `SceneView`: draws a scene through the specified camera
  - `SolidColorView` fills its own area with a single color
- Gfx: `Model` component
- Gfx: `CommandList`, `CommandListRecorder`, `DeviceStateCache`
- Gfx: Commands
  - `SetDirectionalLights`, `SetTransform`, `SetRenderState`,
    `SetTextureStageState`, `SetTexture`, `SetMaterial`, `Draw`, `Clear`
- Gfx: device extensions
  - `ext_dear_imgui_renderer`
  - `ext_x_model_support`
- Gfx: Materials

### Changed
- Renamed runtime to api
- The ClientApp::update method now receives a UpdateContext instead of only
  the delta time
- Input is now part of the update context
- Input: all mouse buttons capture the mouse
- Moved app code into its own folder
- Renamed various public fields, methods and functions
- Updated Dear ImGui (v1.72b -> v1.79)
- Updated EnTT (19ef290 -> v3.5.2)
- Updated fmt (5.3.0 -> 7.1.2)
- Updated spdlog (1.3.1 -> 1.8.1)
  - now compiled as a static library

### Deprecated

### Removed
- gfx: `View`
- Super key
  - the super key is not supported on Windows. It will be added again on other
    platforms once the need arises
- free standing input functions
  - input is now part of the update context
- platform events

### Fixed

### Security

## [0.3.0] - 2020-05-22
### Added
- Basic Scene API with an entity component system (entt)
- Basic Camera class
- HandlePool to allocate and reuse memory slots accessed via a handle
- math::Mat4::Rotation(Vec3) and math::Mat4::Scale(Vec3)
- support for the vertical mouse wheel (MouseScrolledEvent)
- enum_cast: cast an enumerator to its value of the underlying type
- ColorEncoding
- Dear ImGui
- BASALT_ASSERT_MSG
- [sandbox] D3D9 Tutorial 1: Creating a Device
- [sandbox] D3D9 Tutorial 6: Using Meshes
- [build] CMake build support for Windows x86-64
- [build-vs] RelWithDebInfo configuration
- [build-vs] BASALT_DEV_BUILD macro for Debug and RelWithDebInfo builds

### Changed
- Refactored platform events
- Refactored handles
- Refactored project structure
- Renamed BS_BUILD to BASALT_BUILD
- Renamed BS_DEBUG_BUILD to BASALT_DEBUG_BUILD
- Replace BS_APP_DEBUG_BUILD with BASALT_DEBUG_BUILD
- BASALT_ASSERT takes only one parameter
  - use the new BASALT_ASSERT_MSG for the old behaviour

### Removed
- Mat4::Camera (use gfx::Camera instead)

### Fixed
- AltGr leaves a pushed down Ctrl key behind

## [0.2.0] - 2019-08-06
### Added
- Windows x86-64 build support

### Changed
- Renamed WindowMode enum constants
  - FULLSCREEN to FULLSCREEN_EXCLUSIVE
  - FULLSCREEN_BORDERLESS to FULLSCREEN
- renamed WindowDesc in Config from mainWindow to window
- moved Key and MouseButton enumerations from common/Types.h into their
  respective event header files

### Removed
- Left/right variants of the shift, control, super and alt keys
- Repeat count for KeyPressedEvent

### Fixed
- Null pointer dereference when destroying a D3D9 renderer
  ([#3](https://github.com/juli27/basaltcpp/issues/3))

## [0.1.0] - 2019-07-01
### Added
- Windows x86 build support
- Polling input

#### Platform
- Basic platform abstraction
- Windows API support
- Main window creation
- Input events

#### Gfx
- Gfx backend (renderer) abstraction
- Direct3D 9 fixed function backend

#### Sandbox
- Direct3D 9 tutorial 2 - 5
- Navigate with the left and right arrow keys between the test cases

[0.4.0]: https://github.com/juli27/basaltcpp/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/juli27/basaltcpp/releases/tag/v0.3.0
[0.2.0]: https://github.com/juli27/basaltcpp/releases/tag/v0.2.0
[0.1.0]: https://github.com/juli27/basaltcpp/releases/tag/v0.1.0
