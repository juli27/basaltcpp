#pragma once

#include <runtime/shared/Size2D.h>

namespace basalt::gfx::backend {

struct IRenderer;

// A gfx context is directly tied to a window. If another gfx context is
// required (e.g. change of rendering API), the window must be recreated.
struct IGfxContext {
  IGfxContext() noexcept = default;

  IGfxContext(const IGfxContext&) = delete;
  IGfxContext(IGfxContext&&) = delete;

  virtual ~IGfxContext() noexcept = default;

  auto operator=(const IGfxContext&) -> IGfxContext& = delete;
  auto operator=(IGfxContext&&) -> IGfxContext& = delete;

  [[nodiscard]]
  virtual auto surface_size() const noexcept -> Size2Du16 = 0;

  [[nodiscard]]
  virtual auto renderer() const noexcept -> IRenderer& = 0;

  virtual void present() = 0;
};

} // basalt::gfx::backend
