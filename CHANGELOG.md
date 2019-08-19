# Changelog
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## *[0.3.0] - 2019-09-02* (**Unreleased**)
### Added
- Basic Scene API with an entity component system (entt)
- Basic Camera class
- HandlePool to allocate and reuse memory slots accessed via a handle
- math::Mat4::Rotation(Vec3) and math::Mat4::Scale(Vec3)
- support for the vertical mouse wheel (MouseScrolledEvent)

### Changed

### Deprecated

### Removed
- Mat4::Camera (use gfx::Camera instead)

### Fixed
- AltGr leaves a pushed down Ctrl key behind

### Security

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

[0.3.0]: https://github.com/juli27/basaltcpp/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/juli27/basaltcpp/releases/tag/v0.2.0
[0.1.0]: https://github.com/juli27/basaltcpp/releases/tag/v0.1.0
