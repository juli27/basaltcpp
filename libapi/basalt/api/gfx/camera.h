#pragma once

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/types.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera(Vector3f32 const& lookAt, Vector3f32 const& up, Angle fov,
         f32 nearPlane, f32 farPlane) noexcept;

  Vector3f32 lookAt;
  Vector3f32 up;
  Angle fov;
  f32 aspectRatio{};
  f32 nearPlane{};
  f32 farPlane{};
};

class CameraEntity final {
public:
  explicit CameraEntity(Entity entity);

  [[nodiscard]] auto entity() const noexcept -> Entity;
  [[nodiscard]] auto get_transform() const noexcept -> Transform&;
  [[nodiscard]] auto get_camera() const noexcept -> Camera&;

  [[nodiscard]] auto world_to_view() const noexcept -> Matrix4x4f32;
  [[nodiscard]] auto view_to_clip() const noexcept -> Matrix4x4f32;

private:
  Entity mEntity;
};

} // namespace basalt::gfx
