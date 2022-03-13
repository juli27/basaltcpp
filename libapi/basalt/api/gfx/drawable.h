#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/rectangle.h>
#include <basalt/api/math/types.h>

#include <tuple>

namespace basalt::gfx {

struct Drawable {
  Drawable(const Drawable&) = default;
  Drawable(Drawable&&) noexcept = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable&) -> Drawable& = default;
  auto operator=(Drawable&&) noexcept -> Drawable& = default;

  auto draw(ResourceCache& cache, const Size2Du16 viewport,
            const RectangleU16& clip) -> std::tuple<CommandList, RectangleU16> {
    return on_draw(cache, viewport, clip);
  }

protected:
  Drawable() noexcept = default;

  virtual auto on_draw(ResourceCache&, Size2Du16 viewport,
                       const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> = 0;
};

} // namespace basalt::gfx
