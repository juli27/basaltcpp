#pragma once
#ifndef D3D9_TUTORIALS_LIGHTS_H
#define D3D9_TUTORIALS_LIGHTS_H

#include "sandbox/TestCase.h"

#include <runtime/Scene.h>
#include <runtime/shared/Types.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9_tuts {

struct Lights final : TestCase {
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
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  basalt::f32 mLightAngle = 0.0f;
  entt::entity mCylinderEntity = entt::null;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_LIGHTS_H
