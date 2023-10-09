#pragma once

#include <basalt/gfx/backend/ext/texture_3d_support.h>

#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>

namespace basalt::gfx::ext {

class D3D9Texture3DSupport final : public Texture3DSupport {
public:
  static auto create(D3D9Device*) -> D3D9Texture3DSupportPtr;

  [[nodiscard]] auto load(std::filesystem::path const&) -> Texture override;

  explicit D3D9Texture3DSupport(D3D9Device*);

private:
  D3D9Device* mDevice;
};

} // namespace basalt::gfx::ext
