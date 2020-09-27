#pragma once

namespace basalt::gfx {

struct Composite;
struct Device;
struct DrawTarget;

struct Compositor final {
  Compositor() = delete;

  Compositor(const Compositor&) = delete;
  Compositor(Compositor&&) = delete;

  ~Compositor() = delete;

  auto operator=(const Compositor&) -> Compositor& = delete;
  auto operator=(Compositor&&) -> Compositor& = delete;

  static auto compose(Device&, const DrawTarget&) -> Composite;
};

} // namespace basalt::gfx
