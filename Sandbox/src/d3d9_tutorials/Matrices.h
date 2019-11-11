#pragma once
#ifndef D3D9_TUTORIALS_MATRICES_H
#define D3D9_TUTORIALS_MATRICES_H

#include "../ITestCase.h"

#include <basalt/Basalt.h>

#include <memory>

namespace d3d9_tuts {

struct Matrices final : ITestCase {
  Matrices();
  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;
  ~Matrices() = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<basalt::Scene> mScene;
  entt::entity mTriangleEntity;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_MATRICES_H
