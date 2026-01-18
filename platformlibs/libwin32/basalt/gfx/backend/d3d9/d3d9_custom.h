#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#include <wrl/client.h>

#define D3D_DISABLE_9EX
#include <d3d9.h>

#include <d3dx9.h>

namespace basalt::gfx {

namespace detail {

using Microsoft::WRL::ComPtr;

} // namespace detail

using IDirect3D9Ptr = detail::ComPtr<IDirect3D9>;
using IDirect3DDevice9Ptr = detail::ComPtr<IDirect3DDevice9>;
using IDirect3DSwapChain9Ptr = detail::ComPtr<IDirect3DSwapChain9>;
using IDirect3DVertexBuffer9Ptr = detail::ComPtr<IDirect3DVertexBuffer9>;
using IDirect3DIndexBuffer9Ptr = detail::ComPtr<IDirect3DIndexBuffer9>;
using IDirect3DBaseTexture9Ptr = detail::ComPtr<IDirect3DBaseTexture9>;
using IDirect3DTexture9Ptr = detail::ComPtr<IDirect3DTexture9>;
using IDirect3DCubeTexture9Ptr = detail::ComPtr<IDirect3DCubeTexture9>;
using IDirect3DVolumeTexture9Ptr = detail::ComPtr<IDirect3DVolumeTexture9>;

using ID3DXBufferPtr = Microsoft::WRL::ComPtr<ID3DXBuffer>;
using ID3DXEffectPtr = Microsoft::WRL::ComPtr<ID3DXEffect>;
using ID3DXMeshPtr = Microsoft::WRL::ComPtr<ID3DXMesh>;

} // namespace basalt::gfx

#if BASALT_IS_DEV_BUILD
#define PIX_BEGIN_EVENT(color, name) D3DPERF_BeginEvent(color, name)
#define PIX_END_EVENT() D3DPERF_EndEvent()
#define PIX_SET_MARKER(color, name) D3DPERF_SetMarker(color, name)
#else // !BASALT_IS_DEV_BUILD
#define PIX_BEGIN_EVENT(color, name)
#define PIX_END_EVENT()
#define PIX_SET_MARKER(color, name)
#endif
