#pragma once
#ifndef SANDBOX_D3D9_LIGHTS_H
#define SANDBOX_D3D9_LIGHTS_H

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Lights final : TestCase {
  Lights() = delete;
  explicit Lights(basalt::gfx::backend::IRenderer*);

  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;

  ~Lights() override = default;

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights&&) -> Lights& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  basalt::f32 mLightAngle {0.0f};
  entt::entity mCylinder {entt::null};
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_LIGHTS_H
