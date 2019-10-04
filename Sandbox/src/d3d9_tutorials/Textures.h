#pragma once
#ifndef D3D9_TUTORIALS_TEXTURES_H
#define D3D9_TUTORIALS_TEXTURES_H

#include "../ITestCase.h"

#include <basalt/Basalt.h>

#include <memory>

namespace d3d9_tuts {

struct Textures final : ITestCase {
  Textures();
  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;
  ~Textures() = default;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<bs::Scene> mScene;
  entt::entity mCylinderEntity;
};

} // namespace d3d9_tuts

#endif // !D3D9_TUTORIALS_TEXTURES_H
