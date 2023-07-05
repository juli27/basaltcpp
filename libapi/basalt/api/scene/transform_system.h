#pragma once

#include <basalt/api/scene/system.h>

#include <basalt/api/scene/types.h>

namespace basalt {

class TransformSystem final : public System {
public:
  TransformSystem() noexcept = default;

  auto on_update(const SystemContext&) -> void override;
};

} // namespace basalt
