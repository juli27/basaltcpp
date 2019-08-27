#pragma once
#ifndef BS_PLATFORM_IGFX_CONTEXT_H
#define BS_PLATFORM_IGFX_CONTEXT_H

#include <basalt/gfx/backend/IRenderer.h>

namespace basalt::platform {

// A gfx context is directly tied to a window. If another gfx context is
// required (e.g. change of rendering API), the window must be recreated.
struct IGfxContext {
  IGfxContext() = default;
  IGfxContext(const IGfxContext&) = delete;
  IGfxContext(IGfxContext&&) = delete;
  virtual ~IGfxContext() = default;

  auto operator=(const IGfxContext&) -> IGfxContext& = delete;
  auto operator=(IGfxContext&&) -> IGfxContext& = delete;

  virtual auto create_renderer() -> gfx::backend::IRenderer* = 0;
  virtual void present() = 0;
};

} // basalt::platform

#endif // BS_PLATFORM_IGFX_CONTEXT_H