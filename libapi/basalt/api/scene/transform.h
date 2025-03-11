#pragma once

#include <basalt/api/scene/types.h>

#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/types.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <vector>

namespace basalt {

struct Transform final {
  Vector3f32 position;
  Vector3f32 rotation;
  Vector3f32 scale{1.0f};

  // TODO: wrap vectors in Position/Rotation/Scale class?
  auto move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept -> void;
  auto rotate(Angle offsetX, Angle offsetY, Angle offsetZ) noexcept -> void;
  auto rotate_x(Angle) noexcept -> void;
  auto rotate_y(Angle) noexcept -> void;
  auto rotate_z(Angle) noexcept -> void;

  [[nodiscard]] auto to_matrix() const -> Matrix4x4f32;
};

struct LocalToWorld final {
  Matrix4x4f32 matrix{Matrix4x4f32::identity()};
};

struct Parent final {
  EntityId id{};
};

// don't add manually. Use Parent instead
struct Children final {
  std::vector<EntityId> ids;
};

} // namespace basalt
