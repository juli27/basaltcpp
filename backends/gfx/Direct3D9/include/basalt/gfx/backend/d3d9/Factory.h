#pragma once
#ifndef BS_GFX_BACKEND_D3D9_FACTORY_H
#define BS_GFX_BACKEND_D3D9_FACTORY_H

#include <basalt/gfx/backend/IRenderer.h>

#include "D3D9Header.h"

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {

void Shutdown();

IRenderer* CreateRenderer(HWND window);

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_D3D9_FACTORY_H
