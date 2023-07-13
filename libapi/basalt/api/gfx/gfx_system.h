#pragma once

#include <basalt/api/scene/system.h>

#include <entt/core/hashed_string.hpp>

namespace basalt::gfx {

class GfxSystem final : public System {
public:
  static constexpr entt::id_type sMainCamera {
    entt::hashed_string {"main camera"}};

  GfxSystem() noexcept = default;

  auto on_update(const UpdateContext&) -> void override;
};

} // namespace basalt::gfx
