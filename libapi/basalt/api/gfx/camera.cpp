#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/math/matrix4x4.h>

namespace basalt::gfx {

Camera::Camera(const Vector3f32& lookAt, const Vector3f32& up, const Angle fov,
               const f32 nearPlane, const f32 farPlane) noexcept
  : lookAt {lookAt}
  , up {up}
  , fov {fov}
  , nearPlane {nearPlane}
  , farPlane {farPlane} {
}

CameraEntity::CameraEntity(const Entity entity) : mEntity {entity} {
}

auto CameraEntity::entity() const noexcept -> Entity {
  return mEntity;
}

auto CameraEntity::get_transform() const noexcept -> Transform& {
  return mEntity.get<Transform>();
}

auto CameraEntity::get_camera() const noexcept -> Camera& {
  return mEntity.get<Camera>();
}

auto CameraEntity::world_to_view() const noexcept -> Matrix4x4f32 {
  const Transform& transform {get_transform()};
  const Camera& camera {get_camera()};

  return Matrix4x4f32::look_at_lh(transform.position, camera.lookAt, camera.up);
}

auto CameraEntity::view_to_clip() const noexcept -> Matrix4x4f32 {
  const Camera& camera {get_camera()};

  return Matrix4x4f32::perspective_projection(
    camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
}

} // namespace basalt::gfx
