#pragma once

#include <memory>

namespace basalt::gfx {

class D3D9Device;
using D3D9DevicePtr = std::shared_ptr<D3D9Device>;

struct D3D9Factory;
using D3D9FactoryPtr = std::unique_ptr<D3D9Factory>;

} // namespace basalt::gfx
