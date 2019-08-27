#pragma once
#ifndef BS_GFX_H
#define BS_GFX_H

#include <memory>

#include <basalt/Scene.h>

#include "backend/IRenderer.h"

namespace basalt::gfx {

void render(backend::IRenderer* renderer, const std::shared_ptr<Scene>& scene);

} // namespace basalt::gfx

#endif // !BS_GFX_H
