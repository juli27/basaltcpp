#pragma once
#ifndef BASALT_GFX_H
#define BASALT_GFX_H

#include "../Scene.h"

#include "backend/IRenderer.h"

#include <memory>

namespace basalt::gfx {

void render(backend::IRenderer* renderer, const std::shared_ptr<Scene>& scene);

} // namespace basalt::gfx

#endif // !BASALT_GFX_H
