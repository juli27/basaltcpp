#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

namespace basalt::gfx {

struct Drawable {
  Drawable(const Drawable&) = default;
  Drawable(Drawable&&) noexcept = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable&) -> Drawable& = default;
  auto operator=(Drawable&&) noexcept -> Drawable& = default;

  auto draw(ResourceCache& cache, const Size2Du16 viewport) -> CommandList {
    return on_draw(cache, viewport);
  }

protected:
  Drawable() noexcept = default;

  virtual auto on_draw(ResourceCache&, Size2Du16 viewport) -> CommandList = 0;
};

} // namespace basalt::gfx
