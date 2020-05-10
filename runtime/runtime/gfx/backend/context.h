#pragma once
#ifndef BASALT_RUNTIME_GFX_BACKEND_CONTEXT_H
#define BASALT_RUNTIME_GFX_BACKEND_CONTEXT_H

#include "runtime/gfx/backend/IRenderer.h"

#include <memory>

namespace basalt::gfx::backend {

// A gfx context is directly tied to a window. If another gfx context is
// required (e.g. change of rendering API), the window must be recreated.
struct IGfxContext {
  IGfxContext() = default;
  IGfxContext(const IGfxContext&) = delete;
  IGfxContext(IGfxContext&&) = delete;
  virtual ~IGfxContext() = default;

  auto operator=(const IGfxContext&) -> IGfxContext& = delete;
  auto operator=(IGfxContext&&) -> IGfxContext& = delete;

  virtual auto create_renderer() -> std::unique_ptr<IRenderer> = 0;
  virtual void present() = 0;
};

} // basalt::gfx::backend

#endif // BASALT_RUNTIME_GFX_BACKEND_CONTEXT_H
