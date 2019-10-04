#pragma once
#ifndef D3D9_TUTORIALS_VERTICES_H
#define D3D9_TUTORIALS_VERTICES_H

#include "../ITestCase.h"

#include <basalt/Basalt.h>

#include <memory>

namespace d3d9_tuts {

struct Vertices final : ITestCase {
  Vertices();
  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;
  virtual ~Vertices() = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<bs::Scene> mScene;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_VERTICES_H
