#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <vector>

namespace basalt::gfx {

struct Drawable {
  Drawable(const Drawable&) = default;
  Drawable(Drawable&&) noexcept = default;

  virtual ~Drawable() noexcept = default;

  auto operator=(const Drawable&) -> Drawable& = default;
  auto operator=(Drawable&&) noexcept -> Drawable& = default;

  struct DrawContext final {
    std::vector<CommandList>& commandLists;
    ResourceCache& cache;
    Size2Du16 viewport;
  };

  void draw(const DrawContext& context) {
    on_draw(context);
  }

protected:
  Drawable() noexcept = default;

  virtual void on_draw(const DrawContext&) = 0;
};

} // namespace basalt::gfx
