#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace tribase {

struct Textures final : TestCase {
  explicit Textures(basalt::Engine&);

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() override = default;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

  [[nodiscard]] auto name() -> std::string_view override;
  [[nodiscard]] auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(basalt::Engine&) override;

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  basalt::gfx::Material mPointSampler;
  basalt::gfx::Material mLinearSamplerWithMip;
  basalt::gfx::Material mAnisotropicSampler;
  basalt::f64 mTimeAccum {};
};

} // namespace tribase
