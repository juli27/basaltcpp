#pragma once

#include "types.h"
#include "backend/types.h"

#include "api/shared/types.h"

#include <optional>

namespace basalt::gfx {

struct Drawable {
  Drawable(const Drawable&) = default;
  Drawable(Drawable&&) = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable&) -> Drawable& = default;
  auto operator=(Drawable &&) -> Drawable& = default;

  virtual auto draw(ResourceCache&, Size2Du16 viewport) -> CommandList = 0;

  [[nodiscard]] virtual auto clear_color() const -> std::optional<Color> = 0;

protected:
  Drawable() = default;
};

} // namespace basalt::gfx
