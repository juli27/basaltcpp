#pragma once
#ifndef SCENES_D3D9_TUTORIALS_TEXTURES_H
#define SCENES_D3D9_TUTORIALS_TEXTURES_H

#include <Basalt.h>

#include <IScene.h>

namespace scenes {
namespace d3d9tuts {

class TexturesScene final : public IScene {
private:
  struct Cylinder final {
    bs::gfx::backend::MeshHandle mesh;
    bs::gfx::backend::TextureHandle texture;
    bs::f32 angle;
  };

public:

  TexturesScene(bs::gfx::backend::IRenderer* renderer);

  virtual void OnUpdate() override;

private:
  bs::gfx::backend::IRenderer* m_renderer;
  Cylinder m_cylinder;
  bs::math::Mat4f32 m_projection;
  bs::math::Mat4f32 m_view;
};

} // namespace d3d9tuts
} // namespace scenes

#endif // !SCENES_D3D9_TUTORIALS_TEXTURES_H
