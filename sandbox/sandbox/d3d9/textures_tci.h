#pragma once
#ifndef SANDBOX_D3D9_TEXTURES_TCI_H
#define SANDBOX_D3D9_TEXTURES_TCI_H

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/scene/scene.h>
#include <runtime/shared/Size2D.h>

#include <memory>

namespace d3d9 {

struct TexturesTci final : TestCase {
  TexturesTci() = delete;
  TexturesTci(basalt::gfx::backend::IRenderer*, basalt::Size2Du16 windowSize);

  TexturesTci(const TexturesTci&) = delete;
  TexturesTci(TexturesTci&&) = delete;

  ~TexturesTci() override = default;

  auto operator=(const TexturesTci&) -> TexturesTci& = delete;
  auto operator=(TexturesTci&&) -> TexturesTci& = delete;

  auto view(basalt::Size2Du16 windowSize) -> basalt::gfx::SceneView override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  entt::entity mCylinder {entt::null};
};

} // namespace d3d9

#endif // !SANDBOX_D3D9_TEXTURES_TCI_H
