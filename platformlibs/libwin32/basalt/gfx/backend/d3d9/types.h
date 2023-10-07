#pragma once

#include <memory>

namespace basalt::gfx {

class D3D9Factory;
using D3D9FactoryPtr = std::unique_ptr<D3D9Factory>;

class D3D9Device;
using D3D9DevicePtr = std::shared_ptr<D3D9Device>;

class D3D9SwapChain;
using D3D9SwapChainPtr = std::shared_ptr<D3D9SwapChain>;

namespace ext {

class D3D9XEffects;
using D3D9XEffectsPtr = std::shared_ptr<D3D9XEffects>;

class D3D9XEffect;

class D3D9ImGuiRenderer;
using D3D9ImGuiRendererPtr = std::shared_ptr<D3D9ImGuiRenderer>;

class D3D9Texture3DSupport;
using D3D9Texture3DSupportPtr = std::shared_ptr<D3D9Texture3DSupport>;

class D3D9XModelSupport;
using D3D9XModelSupportPtr = std::shared_ptr<D3D9XModelSupport>;

} // namespace ext

} // namespace basalt::gfx
