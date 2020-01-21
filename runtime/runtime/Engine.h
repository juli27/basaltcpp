#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

#include "Scene.h"
#include "gfx/backend/IRenderer.h"
#include "shared/Types.h"

#include <memory>

namespace basalt {

void run();
void quit();
auto get_delta_time() -> f64;
void set_current_scene(const std::shared_ptr<Scene>& scene);
auto get_renderer() -> gfx::backend::IRenderer*;

} // namespace basalt

#endif // !BASALT_ENGINE_H
