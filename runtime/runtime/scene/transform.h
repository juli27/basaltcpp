#pragma once
#ifndef BASALT_RUNTIME_SCENE_TRANSFORM_H
#define BASALT_RUNTIME_SCENE_TRANSFORM_H

#include "runtime/math/Vec3.h"
#include "runtime/shared/Types.h"

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

  math::Vec3f32 mPosition;
  math::Vec3f32 mRotation;
  math::Vec3f32 mScale = {1.0f, 1.0f, 1.0f};
};

} // namespace basalt

#endif // BASALT_RUNTIME_SCENE_TRANSFORM_H
