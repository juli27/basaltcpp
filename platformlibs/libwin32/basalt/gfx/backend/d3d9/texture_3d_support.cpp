#include <basalt/gfx/backend/d3d9/texture_3d_support.h>

#include <basalt/gfx/backend/d3d9/device.h>

#include <utility>

namespace basalt::gfx::ext {

using std::filesystem::path;

auto D3D9Texture3DSupport::create(D3D9DevicePtr device)
  -> D3D9Texture3DSupportPtr {
  return std::make_shared<D3D9Texture3DSupport>(std::move(device));
}

auto D3D9Texture3DSupport::load(path const& path) -> Texture {
  return mDevice->load_texture_3d(path);
}

D3D9Texture3DSupport::D3D9Texture3DSupport(D3D9DevicePtr device)
  : mDevice{std::move(device)} {
}

} // namespace basalt::gfx::ext
