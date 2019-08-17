#pragma once
#ifndef SCENES_D3D9_TUTORIAL_VERTICES_H
#define SCENES_D3D9_TUTORIAL_VERTICES_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {


class Vertices final : public ITestCase {
public:
  Vertices();

public:
  virtual void OnShow() override;
  virtual void OnHide() override;
  virtual void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
};

} // namespace d3d9_tuts

#endif // !SCENES_D3D9_TUTORIAL_VERTICES_H
