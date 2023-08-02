#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

namespace samples {

class Cubes final : public basalt::View {
public:
  explicit Cubes(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
};

} // namespace samples