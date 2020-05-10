#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

#include "runtime/shared/Types.h"

#include <memory>

namespace basalt {

struct IApplication;
struct Scene;

namespace gfx::backend {

struct IGfxContext;
struct IRenderer;

} // gfx::backend

void startup(gfx::backend::IGfxContext*);
void shutdown();

void run(IApplication* app, gfx::backend::IGfxContext* ctx);
void quit();
auto get_delta_time() -> f64;
void set_current_scene(std::shared_ptr<Scene> scene);
auto get_renderer() -> gfx::backend::IRenderer*;

} // namespace basalt

#endif // !BASALT_ENGINE_H
