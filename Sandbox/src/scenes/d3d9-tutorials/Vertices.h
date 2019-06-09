#pragma once
#ifndef SCENES_D3D9_TUTORIAL_VERTICES_H
#define SCENES_D3D9_TUTORIAL_VERTICES_H

#include <Basalt.h>

#include <IScene.h>

namespace scenes {
namespace d3d9tuts {

class VerticesScene final : public IScene {
public:

  VerticesScene(bs::gfx::backend::IRenderer* renderer);

  virtual void OnUpdate() override;

private:
  bs::gfx::backend::IRenderer* m_renderer;
  bs::gfx::backend::MeshHandle m_triangleMesh;
};

} // namespace d3d9tuts
} // namespace scenes

#endif // !SCENES_D3D9_TUTORIAL_VERTICES_H
