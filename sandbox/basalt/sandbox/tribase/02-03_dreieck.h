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

  Dreieck(const Dreieck&) = delete;
  Dreieck(Dreieck&&) = delete;

  ~Dreieck() override = default;

  auto operator=(const Dreieck&) -> Dreieck& = delete;
  auto operator=(Dreieck&&) -> Dreieck& = delete;

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(basalt::Engine&) override;

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  entt::handle mEntity;
  basalt::gfx::Mesh mTriangleMesh;
  basalt::gfx::Mesh mQuadMesh;
  basalt::gfx::Material mSolidMaterial;
  basalt::gfx::Material mWireframeMaterial;
  basalt::f64 mTimeAccum {};
  basalt::i32 mCurrentExercise {};
};

} // namespace tribase
