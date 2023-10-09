#pragma once

#include <basalt/api/scene/system.h>
#include <basalt/api/scene/types.h>

#include <entt/core/hashed_string.hpp>

namespace basalt::gfx {

class GfxSystem final : public System {
public:
  using UpdateAfter = TransformSystem;

  static constexpr auto sMainCamera = entt::hashed_string::value("main camera");

  GfxSystem() noexcept = default;

  auto on_update(UpdateContext const&) -> void override;
};

} // namespace basalt::gfx
