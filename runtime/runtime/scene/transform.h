#pragma once

#include "runtime/math/vec3.h"
#include "runtime/shared/types.h"

namespace basalt {

struct Transform {
  constexpr Transform() noexcept = default;

  constexpr Transform(const Transform&) noexcept = default;
  constexpr Transform(Transform&&) noexcept = default;

  ~Transform() noexcept = default;

  auto operator=(const Transform&) noexcept -> Transform& = default;
  auto operator=(Transform&&) noexcept -> Transform& = default;

  // TODO: wrap vectors in Position/Rotation/Scale class?
  void move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept;
  void rotate(f32 radOffsetX, f32 radOffsetY, f32 radOffsetZ) noexcept;

  Vec3f32 position {};
  Vec3f32 rotation {};
  Vec3f32 scale {1.0f, 1.0f, 1.0f};
};

} // namespace basalt
