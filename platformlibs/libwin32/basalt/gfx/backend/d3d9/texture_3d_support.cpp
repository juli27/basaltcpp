#include <basalt/gfx/backend/d3d9/texture_3d_support.h>

#include <basalt/gfx/backend/d3d9/device.h>

namespace basalt::gfx::ext {

using std::filesystem::path;

auto D3D9Texture3DSupport::create(D3D9Device* device)
  -> D3D9Texture3DSupportPtr {
  return std::make_shared<D3D9Texture3DSupport>(device);
}

auto D3D9Texture3DSupport::load(const path& path) -> Texture {
  return mDevice->load_texture_3d(path);
}

D3D9Texture3DSupport::D3D9Texture3DSupport(D3D9Device* device)
  : mDevice {device} {
}

} // namespace basalt::gfx::ext
