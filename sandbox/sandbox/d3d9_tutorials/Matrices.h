#pragma once
#ifndef D3D9_TUTORIALS_MATRICES_H
#define D3D9_TUTORIALS_MATRICES_H

#include "sandbox/TestCase.h"

#include <runtime/Scene.h>
#include <runtime/gfx/backend/IRenderer.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9_tuts {

struct Matrices final : TestCase {
  Matrices() = delete;
  Matrices(basalt::gfx::backend::IRenderer*);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() override = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  entt::entity mTriangleEntity = entt::null;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_MATRICES_H
