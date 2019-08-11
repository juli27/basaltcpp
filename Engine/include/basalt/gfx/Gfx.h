#pragma once
#ifndef BS_GFX_H
#define BS_GFX_H

#include "backend/IRenderer.h"

namespace basalt::gfx {


void Init();


void Shutdown();


void Render();


void Present();


backend::IRenderer* GetRenderer();

} // namespace basalt::gfx

#endif // !BS_GFX_H
