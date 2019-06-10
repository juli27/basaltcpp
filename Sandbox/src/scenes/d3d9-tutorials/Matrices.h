#pragma once
#ifndef SCENES_D3D9_TUTORIALS_MATRICES_H
#define SCENES_D3D9_TUTORIALS_MATRICES_H

#include <Basalt.h>

#include <IScene.h>

namespace scenes {
namespace d3d9tuts {

struct Triangle final {
  bs::gfx::backend::MeshHandle mesh;
  bs::f32 angle;
};

class MatricesScene final : public IScene {
public:

  MatricesScene(bs::gfx::backend::IRenderer* renderer);

  virtual void OnUpdate() override;

private:
  bs::gfx::backend::IRenderer* m_renderer;
  Triangle m_triangle;
  bs::math::Mat4f32 m_projection;
  bs::math::Mat4f32 m_view;
};

} // namespace d3d9tuts
} // namespace scenes

#endif // !SCENES_D3D9_TUTORIALS_MATRICES_H
