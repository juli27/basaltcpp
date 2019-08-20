#pragma once
#ifndef D3D9_TUTORIALS_DEVICE_H
#define D3D9_TUTORIALS_DEVICE_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {


class Device final : public ITestCase {
public:
  Device();

public:
  virtual void OnShow() override;
  virtual void OnHide() override;
  virtual void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
};

} // namespace d3d9_tuts

#endif // D3D9_TUTORIALS_DEVICE_H
