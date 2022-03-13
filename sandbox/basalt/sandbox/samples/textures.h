#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <entt/entity/handle.hpp>

#include <array>

namespace samples {

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
struct Textures final : TestCase {
  explicit Textures(basalt::Engine&);

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  entt::handle mQuad;
  basalt::u32 mChosenMaterial {0};
  std::array<basalt::gfx::Material, 9> mMaterials {};

  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;

  void on_tick(basalt::Engine&) override;
};

} // namespace samples
