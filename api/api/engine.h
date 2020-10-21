#pragma once

#include "input.h"
#include "types.h"

#include "gfx/resource_cache.h"
#include "gfx/types.h"

#include "gfx/backend/types.h"
#include "gfx/backend/ext/types.h"

#include "resources/resource_registry.h"
#include "resources/types.h"

#include "shared/types.h"
#include "base/types.h"

#include <memory>
#include <string_view>

namespace basalt {

struct Engine {
  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine &&) -> Engine& = delete;

  [[nodiscard]] auto config() const noexcept -> const Config&;
  [[nodiscard]] auto gfx_device() const -> gfx::DevicePtr;
  [[nodiscard]] auto gfx_context() const noexcept -> gfx::Context&;

  [[nodiscard]] auto mouse_cursor() const noexcept -> MouseCursor;
  void set_mouse_cursor(MouseCursor) noexcept;

  template <typename Resource>
  [[nodiscard]] auto load(std::string_view) const -> Resource;

  [[nodiscard]] auto load(MaterialDescriptor) const -> Material;

protected:
  Config& mConfig;
  std::shared_ptr<ResourceRegistry> mResourceRegistry =
    std::make_shared<ResourceRegistry>();

  std::shared_ptr<gfx::Context> mGfxContext;
  gfx::ResourceCache mGfxResourceCache;

  MouseCursor mMouseCursor {};
  bool mIsDirty {false};

  Engine(Config&, std::shared_ptr<gfx::Context>) noexcept;

  ~Engine() noexcept = default;
};

template <>
[[nodiscard]] auto Engine::load(std::string_view filePath) const
  -> gfx::ext::XModel;

template <>
[[nodiscard]] auto Engine::load(std::string_view filePath) const -> Texture;

struct UpdateContext final {
  Engine& engine;
  gfx::DrawTarget& drawTarget;
  f64 deltaTime {};
  Input input;
};

void quit();

} // namespace basalt
