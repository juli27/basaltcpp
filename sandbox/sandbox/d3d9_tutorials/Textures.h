#pragma once
#ifndef D3D9_TUTORIALS_TEXTURES_H
#define D3D9_TUTORIALS_TEXTURES_H

#include "sandbox/TestCase.h"

#include <runtime/Scene.h>
#include <runtime/gfx/backend/IRenderer.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace d3d9_tuts {

struct Textures final : TestCase {
  Textures() = delete;
  Textures(basalt::gfx::backend::IRenderer*);

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() override = default;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  entt::entity mCylinderEntity = entt::null;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_TEXTURES_H
