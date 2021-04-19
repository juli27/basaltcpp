#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx {

struct Compositor final {
  Compositor() = delete;

  Compositor(const Compositor&) = delete;
  Compositor(Compositor&&) = delete;

  ~Compositor() = delete;

  auto operator=(const Compositor&) -> Compositor& = delete;
  auto operator=(Compositor &&) -> Compositor& = delete;

  static auto compose(ResourceCache&, const DrawTarget&) -> Composite;
};

} // namespace basalt::gfx
