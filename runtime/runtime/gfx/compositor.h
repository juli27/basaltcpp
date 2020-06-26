#pragma once

namespace basalt::gfx {

struct Context;
struct DrawTarget;

struct Compositor final {
  Compositor() = delete;

  Compositor(const Compositor&) = delete;
  Compositor(Compositor&&) = delete;

  ~Compositor() = delete;

  auto operator=(const Compositor&) -> Compositor& = delete;
  auto operator=(Compositor&&) -> Compositor& = delete;

  static void compose(Context&, const DrawTarget&);
};

} // namespace basalt::gfx
