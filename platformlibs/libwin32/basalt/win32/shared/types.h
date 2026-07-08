#pragma once

#include <memory>

namespace basalt::gfx {

class Win32GfxFactory;
using Win32GfxFactoryPtr = std::unique_ptr<Win32GfxFactory>;

} // namespace basalt::gfx
