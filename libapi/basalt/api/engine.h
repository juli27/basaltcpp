#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <memory>
#include <unordered_map>

namespace basalt {

struct Engine {
  Engine(Engine const&) = delete;
  Engine(Engine&&) = delete;

  auto operator=(Engine const&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;

  [[nodiscard]] auto config() const noexcept -> Config const&;
  [[nodiscard]] auto config() noexcept -> Config&;

  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;
  [[nodiscard]] auto gfx_info() const noexcept -> gfx::Info const&;
  [[nodiscard]] auto create_gfx_resource_cache() const -> gfx::ResourceCachePtr;

  [[nodiscard]] auto resource_registry() const noexcept -> ResourceRegistry&;
  [[nodiscard]] auto gfx_resource_cache() noexcept -> gfx::ResourceCache&;

  [[nodiscard]] auto root() const -> ViewPtr const&;
  auto set_root(ViewPtr) -> void;

  [[nodiscard]] auto mouse_cursor() const noexcept -> MouseCursor;
  auto set_mouse_cursor(MouseCursor) noexcept -> void;

  auto set_window_mode(WindowMode) noexcept -> void;

  template <typename T>
  auto get_or_load(Resource) -> T;

protected:
  ResourceRegistryPtr mResourceRegistry{};

  gfx::ContextPtr mGfxContext;
  gfx::ResourceCachePtr mGfxResourceCache;

  std::unordered_map<ResourceId, gfx::Texture> mTextures{};
  std::unordered_map<ResourceId, gfx::ext::XModel> mXModels{};

  MouseCursor mMouseCursor{MouseCursor::Arrow};
  bool mIsDirty{false};

  Engine(Config&, gfx::ContextPtr) noexcept;

  ~Engine() noexcept = default;

private:
  Config& mConfig;
  ViewPtr mRoot;

  [[nodiscard]] auto is_loaded(ResourceId) const -> bool;
};

template <>
auto Engine::get_or_load(Resource) -> gfx::ext::XModel;
template <>
auto Engine::get_or_load(Resource) -> gfx::Texture;

auto quit() -> void;

} // namespace basalt
