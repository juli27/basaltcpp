#pragma once
#ifndef BS_GFX_H
#define BS_GFX_H

#include <memory>

#include <basalt/Scene.h>

#include "backend/IRenderer.h"

namespace basalt::gfx {


void Init();


void Shutdown();


void Render(const std::shared_ptr<Scene>& scene);


void Present();


auto GetRenderer() -> backend::IRenderer*;

} // namespace basalt::gfx

#endif // !BS_GFX_H
