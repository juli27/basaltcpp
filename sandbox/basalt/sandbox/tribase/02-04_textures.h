#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace tribase {

struct Textures final : TestCase {
  explicit Textures(basalt::Engine&);

  [[nodiscard]] auto drawable() -> basalt::gfx::DrawablePtr override;

  void tick(basalt::Engine&) override;

protected:
  auto do_handle_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  basalt::gfx::Material mPointSampler;
  basalt::gfx::Material mLinearSamplerWithMip;
  basalt::gfx::Material mAnisotropicSampler;
  basalt::f64 mTimeAccum {};
};

} // namespace tribase
