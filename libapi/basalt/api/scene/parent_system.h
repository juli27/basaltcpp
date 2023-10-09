#pragma once

#include <basalt/api/scene/system.h>

namespace basalt {

class ParentSystem final : public System {
public:
  ParentSystem() noexcept = default;

  auto on_update(UpdateContext const&) -> void override;
};

} // namespace basalt
