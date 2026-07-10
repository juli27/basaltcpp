#pragma once

#include "types.h"

#include "gfx/info.h"
#include "gfx/types.h"

#include "shared/config.h"

#include <memory>

namespace basalt {

struct Engine {
  Engine(Engine const&) = delete;
  Engine(Engine&&) = default;

  auto operator=(Engine const&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = default;

  [[nodiscard]] auto config() const noexcept -> Config const&;
  [[nodiscard]] auto config() noexcept -> Config&;

  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;
  [[nodiscard]] auto gfx_info() const noexcept -> gfx::Info const&;
  [[nodiscard]] auto create_gfx_resource_cache() const -> gfx::ResourceCachePtr;

  [[nodiscard]] auto root() const -> ViewPtr const&;
  auto set_root(ViewPtr) -> void;

  auto canvas_pointer() const -> CanvasPointer;
  auto set_canvas_pointer(CanvasPointer) -> void;

protected:
  gfx::ContextPtr mGfxContext;

  CanvasPointer mCanvasPointer{CanvasPointer::Arrow};
  bool mIsDirty{false};

  Engine(Config, gfx::ContextPtr) noexcept;

  ~Engine() noexcept = default;

private:
  Config mConfig;
  ViewPtr mRoot;
};

} // namespace basalt
