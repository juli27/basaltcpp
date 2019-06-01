#pragma once
#ifndef BS_GFX_BACKEND_FACTORY_H
#define BS_GFX_BACKEND_FACTORY_H

#include "IRenderer.h"

namespace basalt {
namespace gfx {
namespace backend {

void Shutdown();

IRenderer* CreateRenderer();

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_FACTORY_H
