#include "texture_3d_support.h"

#include "device.h"
#include "d3d9_custom.h"

#include <utility>

namespace basalt::gfx::ext {

using std::filesystem::path;

auto D3D9Texture3DSupport::create(D3D9DevicePtr device)
  -> D3D9Texture3DSupportPtr {
  return std::make_shared<D3D9Texture3DSupport>(std::move(device));
}

auto D3D9Texture3DSupport::load(path const& path) -> TextureHandle {
  auto texture = IDirect3DVolumeTexture9Ptr{};

  // TODO: Mip map count is fixed to 1
  if (FAILED(D3DXCreateVolumeTextureFromFileExW(
        mDevice->device().Get(), path.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, nullptr, nullptr, &texture))) {
    throw std::runtime_error{"loading texture file failed"};
  }

  return mDevice->add_texture(std::move(texture));
}

D3D9Texture3DSupport::D3D9Texture3DSupport(D3D9DevicePtr device)
  : mDevice{std::move(device)} {
}

} // namespace basalt::gfx::ext
