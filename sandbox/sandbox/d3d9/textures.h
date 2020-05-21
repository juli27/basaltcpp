#pragma once
#ifndef SANDBOX_D3D9_TEXTURES_H
#define SANDBOX_D3D9_TEXTURES_H

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/scene/scene.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9 {

struct Textures final : TestCase {
  Textures() = delete;
  explicit Textures(basalt::gfx::backend::IRenderer*);

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() override = default;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  entt::entity mCylinderEntity {entt::null};
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_TEXTURES_H
