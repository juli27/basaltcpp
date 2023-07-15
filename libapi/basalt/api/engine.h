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
  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;

  [[nodiscard]] auto config() const noexcept -> const Config&;
  [[nodiscard]] auto config() noexcept -> Config&;

  [[nodiscard]] auto gfx_info() const noexcept -> const gfx::Info&;
  [[nodiscard]] auto create_gfx_resource_cache() const -> gfx::ResourceCachePtr;

  [[nodiscard]] auto resource_registry() const noexcept -> ResourceRegistry&;
  [[nodiscard]] auto gfx_resource_cache() noexcept -> gfx::ResourceCache&;

  [[nodiscard]] auto root() const -> const ViewPtr&;
  auto set_root(ViewPtr) -> void;

  [[nodiscard]] auto mouse_cursor() const noexcept -> MouseCursor;
  void set_mouse_cursor(MouseCursor) noexcept;

  void set_window_mode(WindowMode) noexcept;

  template <typename T>
  auto get_or_load(Resource) -> T;

protected:
  gfx::Info mGfxInfo;

  ResourceRegistryPtr mResourceRegistry {};

  gfx::ResourceCachePtr mGfxResourceCache;
  gfx::DevicePtr mGfxDevice;

  std::unordered_map<ResourceId, gfx::Texture> mTextures {};
  std::unordered_map<ResourceId, gfx::ext::XModel> mXModels {};

  MouseCursor mMouseCursor {MouseCursor::Arrow};
  bool mIsDirty {false};

  Engine(Config&, gfx::Info, gfx::DevicePtr) noexcept;

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

void quit();

} // namespace basalt
