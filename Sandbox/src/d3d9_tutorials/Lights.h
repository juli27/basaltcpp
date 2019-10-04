#pragma once
#ifndef D3D9_TUTORIALS_LIGHTS_H
#define D3D9_TUTORIALS_LIGHTS_H

#include "../ITestCase.h"

#include <basalt/Basalt.h>

#include <memory>

namespace d3d9_tuts {

struct Lights final : ITestCase {
  Lights();
  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;
  ~Lights() override = default;

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights&&) -> Lights& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<bs::Scene> mScene;
  bs::f32 mLightAngle = 0.0f;
  entt::entity mCylinderEntity;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_LIGHTS_H
