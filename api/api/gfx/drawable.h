#pragma once

#include "types.h"
#include "backend/types.h"

#include "api/shared/types.h"

#include "api/math/types.h"

#include <tuple>

namespace basalt::gfx {

struct Drawable {
  Drawable(const Drawable&) = default;
  Drawable(Drawable&&) = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable&) -> Drawable& = default;
  auto operator=(Drawable &&) -> Drawable& = default;

  virtual auto draw(ResourceCache&, Size2Du16 viewport,
                    const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> = 0;

protected:
  Drawable() = default;
};

} // namespace basalt::gfx
