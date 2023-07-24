#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

namespace basalt::gfx::ext {

class XMeshCommandEncoder final {
public:
  static auto draw_x_mesh(CommandList&, XMesh) -> void;
};

} // namespace basalt::gfx::ext
