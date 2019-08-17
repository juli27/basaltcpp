#pragma once
#ifndef D3D9_TUTORIALS_MATRICES_H
#define D3D9_TUTORIALS_MATRICES_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {

class Matrices final : public ITestCase {
public:

  Matrices();

  virtual void OnShow() override;
  virtual void OnHide() override;
  virtual void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
  entt::entity mTriangleEntity;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_MATRICES_H
