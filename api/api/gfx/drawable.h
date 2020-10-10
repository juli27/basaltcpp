#pragma once

#include "types.h"

#include "api/base/types.h"

#include <optional>

namespace basalt {

struct Color;

template <typename T>
struct Size2D;
using Size2Du16 = Size2D<u16>;

namespace gfx {

struct CommandList;

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

} // namespace gfx
} // namespace basalt
