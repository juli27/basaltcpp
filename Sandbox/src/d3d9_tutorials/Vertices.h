#pragma once
#ifndef D3D9_TUTORIALS_VERTICES_H
#define D3D9_TUTORIALS_VERTICES_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {

struct Vertices final : ITestCase {
  Vertices();
  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;
  virtual ~Vertices() = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

  void OnShow() override;
  void OnHide() override;
  void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_VERTICES_H
