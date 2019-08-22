#pragma once
#ifndef D3D9_TUTORIALS_MATRICES_H
#define D3D9_TUTORIALS_MATRICES_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {

struct Matrices final : ITestCase {
  Matrices();
  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;
  ~Matrices() = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

  void OnShow() override;
  void OnHide() override;
  void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
  entt::entity mTriangleEntity;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_MATRICES_H
