#pragma once

#include <basalt/api/scene/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

class System {
public:
  System(const System&) = delete;
  System(System&&) = delete;

  virtual ~System() noexcept = default;

  auto operator=(const System&) -> System& = delete;
  auto operator=(System&&) -> System& = delete;

  struct UpdateContext final {
    SecondsF32 deltaTime;
    Scene& scene;
  };

  virtual auto on_update(const UpdateContext&) -> void = 0;

protected:
  System() noexcept = default;
};

} // namespace basalt
