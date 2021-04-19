#pragma once

#include <memory>

namespace basalt::gfx {

struct D3D9Factory;
using D3D9FactoryPtr = std::unique_ptr<D3D9Factory>;

} // namespace basalt::gfx
