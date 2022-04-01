#pragma once

#include <basalt/api/view.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/types.h>

#include <entt/entity/fwd.hpp>

namespace samples {

class SimpleScene final : public basalt::View {
public:
  explicit SimpleScene(basalt::Engine&);

  SimpleScene(const SimpleScene&) = delete;
  SimpleScene(SimpleScene&&) = delete;

  ~SimpleScene() noexcept override;

  auto operator=(const SimpleScene&) -> SimpleScene& = delete;
  auto operator=(SimpleScene&&) -> SimpleScene& = delete;

private:
  basalt::gfx::ResourceCache& mGfxResources;
  basalt::ScenePtr mScene;
  basalt::gfx::Mesh mTriangleMesh;
  entt::entity mTriangle {};

  auto on_tick(basalt::Engine&) -> void override;
};

} // namespace samples
