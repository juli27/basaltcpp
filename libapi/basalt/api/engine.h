#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/resource_registry.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/types.h>

#include <memory>

namespace basalt {

struct Engine {
  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;

  [[nodiscard]] auto config() const noexcept -> const Config&;

  [[nodiscard]] auto resource_registry() const noexcept -> ResourceRegistry&;
  [[nodiscard]] auto gfx_resource_cache() noexcept -> gfx::ResourceCache&;

  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;

  [[nodiscard]] auto delta_time() const noexcept -> f64;
  [[nodiscard]] auto window_surface_size() const -> Size2Du16;

  void set_window_surface_content(gfx::DrawablePtr);

  void push_input_layer(InputLayerPtr);

  [[nodiscard]] auto mouse_cursor() const noexcept -> MouseCursor;
  void set_mouse_cursor(MouseCursor) noexcept;

  void set_window_mode(WindowMode) noexcept;

  void load(Resource);

  template <typename T>
  auto get_or_load(Resource) -> T;

  template <typename T>
  auto get(ResourceId) -> T;

protected:
  Config& mConfig;
  ResourceRegistryPtr mResourceRegistry {std::make_shared<ResourceRegistry>()};

  gfx::ContextPtr mGfxContext;
  gfx::ResourceCache mGfxResourceCache;

  gfx::DrawablePtr mWindowSurfaceContent;
  std::vector<InputLayerPtr> mInputLayers;

  f64 mDeltaTime {};

  MouseCursor mMouseCursor {MouseCursor::Arrow};
  bool mIsDirty {false};

  Engine(Config&, gfx::ContextPtr) noexcept;

  ~Engine() noexcept = default;
};

template <>
auto Engine::get_or_load(Resource) -> gfx::ext::XModel;
template <>
auto Engine::get_or_load(Resource) -> gfx::Texture;

template <>
[[nodiscard]] auto Engine::get(ResourceId) -> gfx::ext::XModel;
template <>
[[nodiscard]] auto Engine::get(ResourceId) -> gfx::Texture;

void quit();

} // namespace basalt
