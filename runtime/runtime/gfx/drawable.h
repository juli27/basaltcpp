#pragma once

#include "backend/render_command.h"

#include <runtime/shared/Color.h>
#include <runtime/shared/Size2D.h>

namespace basalt::gfx {

struct Drawable {
  Drawable() = default;

  Drawable(const Drawable& other) = default;
  Drawable(Drawable&& other) = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable& other) -> Drawable& = default;
  auto operator=(Drawable&& other) -> Drawable& = default;

  virtual auto draw(Size2Du16 viewport) -> backend::RenderCommandList = 0;

  // TODO: remove
  [[nodiscard]]
  virtual auto clear_color() const -> Color = 0;
};

} // namespace basalt::gfx
