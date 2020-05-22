#pragma once
#ifndef SANDBOX_D3D9_TEXTURES_TCI_H
#define SANDBOX_D3D9_TEXTURES_TCI_H

#include "sandbox/test_case.h"

#include <runtime/scene/scene.h>
#include <runtime/gfx/backend/IRenderer.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9 {

struct TexturesTci final : TestCase {
  TexturesTci() = delete;
  explicit TexturesTci(basalt::gfx::backend::IRenderer*);

  TexturesTci(const TexturesTci&) = delete;
  TexturesTci(TexturesTci&&) = delete;

  ~TexturesTci() override = default;

  auto operator=(const TexturesTci&) -> TexturesTci& = delete;
  auto operator=(TexturesTci&&) -> TexturesTci& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  entt::entity mCylinderEntity {entt::null};
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_TEXTURES_TCI_H
