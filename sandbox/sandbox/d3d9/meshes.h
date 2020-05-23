#pragma once
#ifndef SANDBOX_D3D9_MESHES_H
#define SANDBOX_D3D9_MESHES_H

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Meshes final : TestCase {
  Meshes() = delete;
  explicit Meshes(basalt::gfx::backend::IRenderer*);

  Meshes(const Meshes&) = delete;
  Meshes(Meshes&&) = delete;

  ~Meshes() override = default;

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(Meshes&&) -> Meshes& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  entt::entity mTiger {entt::null};
};

} // namespace d3d9

#endif // SANDBOX_D3D9_MESHES_H
