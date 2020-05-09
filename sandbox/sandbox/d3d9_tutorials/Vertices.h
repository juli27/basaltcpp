#pragma once
#ifndef SANDBOX_D3D9_VERTICES_H
#define SANDBOX_D3D9_VERTICES_H

#include "sandbox/TestCase.h"

#include <runtime/Scene.h>

#include <memory>

namespace d3d9 {

struct Vertices final : TestCase {
  Vertices();

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() override = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_VERTICES_H
