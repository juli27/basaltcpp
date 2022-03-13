#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <entt/entity/handle.hpp>

namespace tribase {

// TODO: Exercise 2.5.6-1 requires dynamic vertex buffer
// TODO: Figure out what to do about the FillMode RenderState for
// Exercise 2.5.6-3
struct Dreieck final : basalt::View {
  explicit Dreieck(basalt::Engine&);

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

  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;

  void on_tick(basalt::Engine&) override;
};

} // namespace tribase
