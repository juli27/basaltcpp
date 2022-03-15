#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/base/types.h>

#include <entt/entity/handle.hpp>

#include <array>

namespace samples {

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
struct Textures final : basalt::View {
  explicit Textures(basalt::Engine&);

private:
  basalt::ScenePtr mScene;
  basalt::SceneViewPtr mSceneView;
  entt::handle mQuad;
  basalt::u32 mChosenMaterial {0};
  std::array<basalt::gfx::Material, 9> mMaterials {};

  auto on_draw(const DrawContext&) -> void override;

  void on_tick(basalt::Engine&) override;
};

} // namespace samples
