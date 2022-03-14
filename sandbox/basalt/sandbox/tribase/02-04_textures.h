#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace tribase {

struct Textures final : basalt::View {
  explicit Textures(basalt::Engine&);

private:
  basalt::ScenePtr mScene;
  basalt::SceneViewPtr mSceneView;
  basalt::gfx::Material mPointSampler;
  basalt::gfx::Material mLinearSamplerWithMip;
  basalt::gfx::Material mAnisotropicSampler;
  basalt::f64 mTimeAccum {};

  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;

  void on_tick(basalt::Engine&) override;

  auto on_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
