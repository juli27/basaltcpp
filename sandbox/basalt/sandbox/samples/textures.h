#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <entt/entity/handle.hpp>

#include <array>
#include <string_view>

namespace samples {

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
struct Textures final : TestCase {
  explicit Textures(basalt::Engine&);

  [[nodiscard]] auto name() -> std::string_view override;
  [[nodiscard]] auto drawable() -> basalt::gfx::DrawablePtr override;
  void tick(basalt::Engine&) override;

protected:
  auto do_handle_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;

private:
  basalt::ScenePtr mScene;
  basalt::gfx::SceneViewPtr mSceneView;
  entt::handle mQuad;
  basalt::u32 mChosenMaterial {0};
  std::array<basalt::gfx::Material, 9> mMaterials {};
};

} // namespace samples
