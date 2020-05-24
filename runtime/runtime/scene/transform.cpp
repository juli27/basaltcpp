#include "runtime/scene/transform.h"

#include "runtime/math/Constants.h"

namespace basalt {

using math::PI;
using math::Vec3f32;

void Transform::move(const f32 offsetX, const f32 offsetY
                            , const f32 offsetZ) noexcept {
  mPosition += Vec3f32(offsetX, offsetY, offsetZ);
}

void Transform::rotate(const f32 radOffsetX, const f32 radOffsetY
                              , const f32 radOffsetZ) noexcept {
  mRotation += Vec3f32(radOffsetX, radOffsetY, radOffsetZ);

  constexpr auto maxAngle = PI * 2.0f;
  if (mRotation.x < 0.0f) mRotation.x = maxAngle;
  if (mRotation.x > maxAngle) mRotation.x = 0.0f;
  if (mRotation.y < 0.0f) mRotation.y = maxAngle;
  if (mRotation.y > maxAngle) mRotation.y = 0.0f;
  if (mRotation.z < 0.0f) mRotation.z = maxAngle;
  if (mRotation.z > maxAngle) mRotation.z = 0.0f;
}

} // namespace basalt
