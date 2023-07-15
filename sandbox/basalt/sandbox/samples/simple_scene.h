#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

namespace samples {

class SimpleScene final : public basalt::View {
public:
  explicit SimpleScene(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Mesh mMesh;
  basalt::gfx::Material mMaterial;
};

} // namespace samples
