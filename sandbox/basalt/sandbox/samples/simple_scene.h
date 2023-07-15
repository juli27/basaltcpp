#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

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
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Mesh mMesh;
  basalt::gfx::Material mMaterial;
};

} // namespace samples
