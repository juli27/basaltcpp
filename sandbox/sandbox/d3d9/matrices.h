#pragma once
#ifndef SANDBOX_D3D9_MATRICES_H
#define SANDBOX_D3D9_MATRICES_H

#include "sandbox/test_case.h"

#include <runtime/Scene.h>
#include <runtime/gfx/backend/IRenderer.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9 {

struct Matrices final : TestCase {
  Matrices() = delete;
  explicit Matrices(basalt::gfx::backend::IRenderer*);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() override = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  entt::entity mTriangleEntity {entt::null};
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_MATRICES_H
