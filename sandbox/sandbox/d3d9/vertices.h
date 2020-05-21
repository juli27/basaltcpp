#pragma once
#ifndef SANDBOX_D3D9_VERTICES_H
#define SANDBOX_D3D9_VERTICES_H

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Vertices final : TestCase {
  Vertices() = delete;
  explicit Vertices(basalt::gfx::backend::IRenderer*);

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() override = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_VERTICES_H
