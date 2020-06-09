#pragma once

#include "draw_target.h"
#include "backend/context.h"

#include <memory>

namespace basalt::gfx {

struct Drawable;

struct Compositor final {
  Compositor() = delete;
  explicit Compositor(std::shared_ptr<backend::IGfxContext>);

  Compositor(const Compositor&) = delete;
  Compositor(Compositor&&) = default;

  ~Compositor() = default;

  auto operator=(const Compositor&) -> Compositor& = delete;
  auto operator=(Compositor&&) -> Compositor& = delete;

  [[nodiscard]]
  auto draw_target() -> DrawTarget&;

  void compose();

private:
  std::shared_ptr<backend::IGfxContext> mContext {};
  DrawTarget mDrawTarget;
};

} // namespace basalt::gfx
