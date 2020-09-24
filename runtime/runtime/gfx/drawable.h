#pragma once

#include <runtime/shared/types.h>

namespace basalt {

struct Color;

template <typename T>
struct Size2D;
using Size2Du16 = Size2D<u16>;

namespace gfx {

struct CommandList;
struct Device;

struct Drawable {
  Drawable() = default;

  Drawable(const Drawable& other) = default;
  Drawable(Drawable&& other) = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable& other) -> Drawable& = default;
  auto operator=(Drawable&& other) -> Drawable& = default;

  virtual auto draw(Device&, Size2Du16 viewport) -> CommandList = 0;

  // TODO: remove
  [[nodiscard]]
  virtual auto clear_color() const -> Color = 0;
};

} // namespace gfx
} // namespace basalt
