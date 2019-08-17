#pragma once
#ifndef SCENES_D3D9_TUTORIALS_LIGHTS_H
#define SCENES_D3D9_TUTORIALS_LIGHTS_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {


class Lights final : public ITestCase {
public:

  Lights();

  virtual void OnShow() override;
  virtual void OnHide() override;
  virtual void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
  bs::f32 mLightAngle;
  entt::entity mCylinderEntity;
};

} // namespace d3d9_tuts

#endif // !SCENES_D3D9_TUTORIALS_LIGHTS_H
