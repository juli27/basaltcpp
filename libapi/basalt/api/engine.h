#pragma once

#include "types.h"

#include "gfx/info.h"
#include "gfx/types.h"

namespace basalt {

struct Engine {
  Engine(Engine const&) = delete;
  Engine(Engine&&) = default;

  auto operator=(Engine const&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = default;

  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;
  [[nodiscard]] auto gfx_info() const noexcept -> gfx::Info const&;
  [[nodiscard]] auto create_gfx_resource_cache() const -> gfx::ResourceCachePtr;

  [[nodiscard]] auto root() const -> ViewPtr const&;
  auto set_root(ViewPtr) -> void;

protected:
  gfx::ContextPtr mGfxContext;

  explicit Engine(gfx::ContextPtr) noexcept;

  ~Engine() noexcept = default;

private:
  ViewPtr mRoot;
};

} // namespace basalt
