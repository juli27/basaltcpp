# Basalt Engine
## OS Support
### Windows
The supported Windows versions are equivalent to the versions officially
supported by Microsoft.

> The Windows versions in *italics* probably work for now as features not
> available in these versions haven't been used yet.

* *Windows Vista*
* *Windows Vista SP1*
* *Windows Vista SP2*
* *Windows Vista SP2 with Platform Update*
* *Windows 7*
* *Windows 7 SP1*
* **Windows 7 SP1 with Platform Update**
* *Windows 8*
* **Windows 8.1**
* *Windows 10 1507 TH1 (Build 10240)*
* *Windows 10 1511 TH2 November Update (Build 10586)*
* *Windows 10 1607 RS1 Anniversary Update (Build 14393)*
* *Windows 10 1703 RS2 Creators Update (Build 15063)*
* *Windows 10 1709 RS3 Fall Creators Update (Build 16299)*
* **Windows 10 1803 RS4 April 2018 Update (Build 17134)**
* **Windows 10 1809 RS5 October 2018 Update (Build 17763)**
* **Windows 10 1903 19H1 May 2019 Update (Build 18362)**

## Hardware Support (incomplete)
### CPU
* SSE2 instructions
* Intel Sandy Bridge processor

### GPU
Only Desktop cards are supported.

* ATI Radeon X1000 Series or newer
* Nvidia GeForce 6 series or newer
* Oldest still officially supported GPUs
  * AMD Radeon HD 7000 Series

## Roadmap
### v0.1 (2019-07-01)
* Windows x86 support
* Direct3D 9 fixed function renderer
* Very basic scene abstraction
* Model loading with automatic Mesh and Material management

### v0.2 (2019-08-05)
* 64 bit build support
* Windows x86-64 support
* refactor renderer creation into the platform libs

### v0.3 (2019-09-02)
* OpenGL 1.0 renderer
* GLFW backend
* Linux x86-64 support

## Feature TODO
* turn /permissive- back on
* Direct3D 9 programmable pipeline renderer
* 64-bit support
* upgrade Direct3D 9 renderer to Direct3D 9Ex
* OpenGL 1.0 renderer (up to OpenGL 2.0)
* GLES 1.0 renderer
* Linux + Wayland + EGL support
* Android support
* Direct3D 10 renderer
* Direct3D 11 renderer
* Direct3D 12 renderer
* Vulkan 1.0 renderer
* audio support
* controller support
* scene graph
* entity component system
* editor

### Potential Bugs
* WinAPI Keyboard input: what happens when there is no left and right variant
  of SHIFT / ALT

### Windows Vista
* System dpi aware
* Direct3D 9 Ex
* DXGI 1.0
* Direct3D 10
* XInput 9.1.0 (1.3 when using the dll from the DX SDK)
* Microsoft Media Foundation (replaces DirectShow)

### Windows Vista SP1
* Direct3D 10.1

### Windows Vista SP2 with Platform Update
* DXGI 1.1
* Direct3D 11
* Direct2D
* Direct Write

### Windows 7
* Touch Input
* Direct3D 9Ex Flip model

### Windows 7 SP1
* AVX instructions

### Windows 7 SP1 with Platform Update
* DXGI 1.2 (partial)
* Direct3D 11.1 (partial)
* Direct2D 1.1

### Windows 8
* SSE2 required
* Windows Runtime (+ ARM)
* DXGI 1.2
* Direct3D 11.1
* no support for less than 32 bbp device mode (GDI, EnumDisplaySettingsW)
  * emulates 8 bbp and 16 bbp on 32 bbp when not manifested as Windows 8
    compatible
* DWM composition is always enabled
* XInput 1.4

### Windows 8.1
* Per monitor (v1) dpi aware
* DXGI 1.3
* Direct3D 11.2
* Direct2D 1.2

### Windows 10 1507 TH1 (Build 10240)
* DXGI 1.4
* Direct3D 11.3
* Direct3D 12
* Direct2D 1.3

### Windows 10 1511 TH2 November Update (Build 10586)
* Direct3D 11.4

### Windows 10 1607 RS1 Anniversary Update (Build 14393)
* DXGI 1.5
* Direct2D 1.4

### Windows 10 1703 RS2 Creators Update (Build 15063)
* Per monitor (v2) dpi aware
* Direct3D 11.4 Update
* Direct2D 1.5

### Windows 10 1709 RS3 Fall Creators Update (Build 16299)
* Desktop ARM64 support (UWP + Win32)

## Timeline
* (2004-04-14) Nvidia GeForce 6 series
* (2005-10-05) ATI Radeon X1000 series
* (2007-01-30) Windows Vista
* (2007-06-28) ATI Radeon HD 2000 series
* (2007-11-19) ATI Radeon HD 3000 series
* (2008-06-25) ATI Radeon HD 4000 series
* (2009-09-23) ATI Radeon HD 5000 series
* (2010-10-22) AMD Radeon HD 6000 series
* (2012-01-09) AMD Radeon HD 7000 series
* (2013-10-08) AMD Radeon Rx 200 series
* (2015-06-16) AMD Radeon Rx 300 series
* (2016-06-29) AMD Radeon RX 400 series
* (2017-04-18) AMD Radeon RX 500 series
* (2017-08-14) AMD Radeon RX Vega series

## Hardware History
Only Desktop cards are considered.

### ATI/AMD
#### ATI Radeon X1000 series
* D3D FL: 9_3
* D3D SM: 3.0
* OpenGL: 2.0
* Memory: 128MB - 512MB

#### ATI Radeon HD 2000 series
* D3D FL: 10_0
* D3D SM: 4.0
* OpenGL: 3.3
* Memory: 128MB - 1024MB

#### ATI Radeon HD 3000 series
* D3D FL: 10_1
* D3D SM: 4.1
* OpenGL: 3.3
* Memory: 256MB - 1024MB
* Note: Radeon HD 3410 only supports D3D FL 10_0

#### ATI Radeon HD 4000 series
* D3D FL: 10_1
* D3D SM: 4.1
* OpenGL: 3.3
* OpenCL: 1.1
* Memory: 256MB - 2048MB

#### ATI Radeon HD 5000 series
* D3D FL: 11_0
* D3D SM: 5.0
* OpenGL: 4.5
* OpenCL: 1.2
* Memory: 512MB - 4096MB

#### AMD Radeon HD 6000 series
* D3D FL: 11_0
* D3D SM: 5.0
* OpenGL: 4.5
* OpenCL: 1.2
* Memory: 512MB - 4096MB

#### AMD Radeon HD 7000 series
Only GCN cards are considered (Radeon HD 7730+)
* D3D FL: 11_1
* D3D SM: 5.1
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.0
* Memory: 1024MB - 6144MB

#### AMD Radeon Rx 200 series
Only GCN cards are considered (Radeon R5 240+)
* D3D FL: 12_0
* D3D SM: 6.0
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.1
* Memory: 1024MB - 8192MB

#### AMD Radeon Rx 300 series
* D3D FL: 12_0
* D3D SM: 6.0
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.1
* Memory: 1024MB - 8192MB

#### AMD Radeon RX 400 series
* D3D FL: 12_0
* D3D SM: 6.0
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.1
* Memory: 1024MB - 8192MB

#### AMD Radeon RX 500 series
* D3D FL: 12_0
* D3D SM: 6.0
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.1
* Memory: 1024MB - 8192MB

#### AMD Radeon RX Vega series
* D3D FL: 12_1
* D3D SM: 6.3
* OpenGL: 4.6
* OpenCL: 2.0
* Vulkan: 1.1
* Memory: 8GB - 16GB

### Nvidia
#### Nvidia GeForce 6 series
* D3D FL: 9_3
* D3D SM: 3.0
* OpenGL: 2.1
* Memory: 128MB - 512MB
