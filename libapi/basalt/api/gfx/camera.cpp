#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/transform.h>

namespace basalt::gfx {

Camera::Camera(Vector3f32 const& lookAt, Vector3f32 const& up, Angle const fov,
               f32 const nearPlane, f32 const farPlane) noexcept
  : lookAt{lookAt}
  , up{up}
  , fov{fov}
  , nearPlane{nearPlane}
  , farPlane{farPlane} {
}

CameraEntity::CameraEntity(Entity const entity) : mEntity{entity} {
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
  auto const& transform = get_transform();
  auto const& camera = get_camera();

  return Matrix4x4f32::look_at_lh(transform.position, camera.lookAt, camera.up);
}

auto CameraEntity::view_to_clip() const noexcept -> Matrix4x4f32 {
  auto const& camera = get_camera();

  return Matrix4x4f32::perspective_projection(
    camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
}

} // namespace basalt::gfx
