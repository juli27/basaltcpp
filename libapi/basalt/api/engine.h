#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

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

  [[nodiscard]]
  auto mouse_cursor() const noexcept -> MouseCursor;

  auto set_mouse_cursor(MouseCursor) noexcept -> void;

  auto set_window_mode(WindowMode) noexcept -> void;

protected:
  gfx::ContextPtr mGfxContext;

  MouseCursor mMouseCursor{MouseCursor::Arrow};
  bool mIsDirty{false};

  Engine(Config, gfx::ContextPtr) noexcept;

  ~Engine() noexcept = default;

private:
  Config mConfig;
  ViewPtr mRoot;
};

} // namespace basalt
