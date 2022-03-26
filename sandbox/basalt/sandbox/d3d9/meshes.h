#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

namespace d3d9 {

struct Meshes final : basalt::View {
  explicit Meshes(basalt::Engine&);
  Meshes(const Meshes&) = delete;
  Meshes(const Meshes&&) = delete;

  ~Meshes() noexcept override;

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(const Meshes&&) -> Meshes& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::ext::XModel mModel {};
  basalt::gfx::Camera mCamera;
  basalt::f32 mAngleYRad {};

  void on_draw(const DrawContext&) override;
  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
