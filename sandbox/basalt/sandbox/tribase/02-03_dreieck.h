#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <entt/entity/handle.hpp>

namespace tribase {

// TODO: Exercise 2.5.6-1 requires dynamic vertex buffer
// TODO: Figure out what to do about the FillMode RenderState for
// Exercise 2.5.6-3
struct Dreieck final : TestCase {
  explicit Dreieck(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_tick(basalt::Engine&) override;

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  entt::handle mEntity;
  basalt::gfx::Mesh mTriangleMesh;
  basalt::gfx::Mesh mQuadMesh;
  basalt::gfx::Material mSolidMaterial;
  basalt::gfx::Material mWireframeMaterial;
  basalt::gfx::Material mQuadMaterial;
  basalt::f64 mTimeAccum {};
  basalt::i32 mCurrentExercise {};
};

} // namespace tribase
