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

  struct DrawContext final {
    ResourceCache& cache;
    Size2Du16 viewport;
  };

  auto draw(const DrawContext& context) -> CommandList {
    return on_draw(context);
  }

protected:
  Drawable() noexcept = default;

  virtual auto on_draw(const DrawContext&) -> CommandList = 0;
};

} // namespace basalt::gfx
