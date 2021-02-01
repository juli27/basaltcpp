#pragma once

#include "input.h"
#include "types.h"

#include "gfx/resource_cache.h"
#include "gfx/types.h"

#include "gfx/backend/types.h"
#include "gfx/backend/ext/types.h"

#include "shared/resource_registry.h"
#include "shared/types.h"

#include "base/types.h"

#include <memory>

namespace basalt {

struct Engine {
  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine &&) -> Engine& = delete;

  [[nodiscard]] auto config() const noexcept -> const Config&;

  [[nodiscard]] auto resource_registry() const noexcept -> ResourceRegistry&;
  [[nodiscard]] auto gfx_resource_cache() noexcept -> gfx::ResourceCache&;

  [[nodiscard]] auto gfx_device() const -> gfx::DevicePtr;
  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;

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

  std::shared_ptr<gfx::Context> mGfxContext;
  gfx::ResourceCache mGfxResourceCache;

  MouseCursor mMouseCursor {MouseCursor::Arrow};
  bool mIsDirty {false};

  Engine(Config&, std::shared_ptr<gfx::Context>) noexcept;

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

struct UpdateContext final {
  Engine& engine;
  gfx::DrawTarget& drawTarget;
  f64 deltaTime {};
  Input input;
};

void quit();

} // namespace basalt
