#pragma once

#include <basalt/api/scene/system.h>

namespace basalt::gfx {

class GfxSystem final : public System {
public:
  GfxSystem() noexcept = default;

  auto on_update(const UpdateContext&) -> void override;
};

} // namespace basalt::gfx
