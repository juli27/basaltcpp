#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

namespace samples {

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
class Textures final : public basalt::View {
public:
  explicit Textures(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
};

} // namespace samples
