#pragma once
#ifndef BS_ENGINE_H
#define BS_ENGINE_H

#include "Scene.h"
#include "common/Types.h"
#include "gfx/backend/IRenderer.h"

#include <memory>

namespace basalt {

void run();
void quit();
auto get_delta_time() -> f64;
void set_current_scene(const std::shared_ptr<Scene>& scene);
auto get_renderer() -> gfx::backend::IRenderer*;

} // namespace basalt

#endif // !BS_ENGINE_H
