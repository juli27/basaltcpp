#pragma once

#include <basalt/api/scene/system.h>

#include <basalt/api/scene/types.h>

namespace basalt {

class TransformSystem final : public System {
public:
  using UpdateAfter = ParentSystem;

  TransformSystem() noexcept = default;

  auto on_update(UpdateContext const&) -> void override;
};

} // namespace basalt
