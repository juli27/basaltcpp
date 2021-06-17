#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/d3d9/context.h>
#include <basalt/gfx/backend/d3d9/device.h>
#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <fmt/format.h>

#include <array>
#include <string>
#include <utility>

namespace basalt::gfx {

using Microsoft::WRL::ComPtr;

using std::array;
using std::string;
using std::tuple;

namespace {

constexpr array<D3DFORMAT, 4> ALLOWED_DISPLAY_FORMATS {
  D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10};

constexpr array<D3DFORMAT, 6> ALLOWED_BACK_BUFFER_FORMATS {
  D3DFMT_R5G6B5,   D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5,
  D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10};

auto to_image_format(const D3DFORMAT format) -> ImageFormat {
  switch (format) {
  case D3DFMT_A8R8G8B8:
    return ImageFormat::B8G8R8A8;

  case D3DFMT_X8R8G8B8:
    return ImageFormat::B8G8R8X8;

  case D3DFMT_R5G6B5:
    return ImageFormat::B5G6R5;

  case D3DFMT_X1R5G5B5:
    return ImageFormat::B5G5R5X1;

  case D3DFMT_A1R5G5B5:
    return ImageFormat::B5G5R5A1;

  case D3DFMT_A2R10G10B10:
    return ImageFormat::B10G10R10A2;

  default:
    return ImageFormat::Unknown;
  }
}

} // namespace

D3D9Factory::D3D9Factory(ComPtr<IDirect3D9> factory)
  : mFactory {std::move(factory)} {
}

auto D3D9Factory::get_current_adapter_mode(const u32 adapterIndex) const
  -> AdapterMode {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  D3DDISPLAYMODE currentAdapterMode;
  mFactory->GetAdapterDisplayMode(adapterIndex, &currentAdapterMode);

  return AdapterMode {currentAdapterMode.Width, currentAdapterMode.Height,
                      currentAdapterMode.RefreshRate,
                      to_image_format(currentAdapterMode.Format)};
}

auto D3D9Factory::get_adapter_count() const -> u32 {
  return mFactory->GetAdapterCount();
}

auto D3D9Factory::get_adapter_monitor(const u32 adapterIndex) const
  -> HMONITOR {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  return mFactory->GetAdapterMonitor(adapterIndex);
}

auto D3D9Factory::query_adapter_info(const u32 adapterIndex) const
  -> AdapterInfo {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  D3DADAPTER_IDENTIFIER9 adapterIdentifier {};
  D3D9CALL(
    mFactory->GetAdapterIdentifier(adapterIndex, 0ul, &adapterIdentifier));

  u16 product = HIWORD(adapterIdentifier.DriverVersion.HighPart);
  u16 version = LOWORD(adapterIdentifier.DriverVersion.HighPart);
  u16 subVersion = HIWORD(adapterIdentifier.DriverVersion.LowPart);
  u16 build = LOWORD(adapterIdentifier.DriverVersion.LowPart);

  string driverInfo = fmt::format("{} ({}.{}.{}.{})", adapterIdentifier.Driver,
                                  product, version, subVersion, build);

  AdapterCapabilities capabilities {};
  // TODO: figure out the difference between caps from factory and device
  // interface
  D3DCAPS9 d3d9Caps {};
  D3D9CALL(mFactory->GetDeviceCaps(adapterIndex, D3DDEVTYPE_HAL, &d3d9Caps));

  return AdapterInfo {string {adapterIdentifier.Description},
                      std::move(driverInfo), query_adapter_modes(adapterIndex),
                      adapterIndex, capabilities};
}

auto D3D9Factory::query_adapter_modes(const u32 adapterIndex) const
  -> AdapterModeList {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  AdapterModeList adapterModes {};

  for (const D3DFORMAT format : ALLOWED_DISPLAY_FORMATS) {
    const u32 count = mFactory->GetAdapterModeCount(adapterIndex, format);

    adapterModes.reserve(adapterModes.size() + count);
    const ImageFormat surfaceFormat = to_image_format(format);

    for (u32 i = 0; i < count; i++) {
      D3DDISPLAYMODE adapterMode;
      D3D9CALL(
        mFactory->EnumAdapterModes(adapterIndex, format, i, &adapterMode));

      // EnumAdapterModes treats pixel formats 565 and 555 as equivalent, and
      // returns the correct version
      if (adapterMode.Format != format) {
        break;
      }

      adapterModes.emplace_back(
        AdapterMode {adapterMode.Width, adapterMode.Height,
                     adapterMode.RefreshRate, surfaceFormat});
    }
  }

  return adapterModes;
}

auto D3D9Factory::create_device_and_context(
  const HWND window, const DeviceAndContextDesc& desc) const
  -> tuple<DevicePtr, ContextPtr> {
  BASALT_ASSERT(desc.adapterIndex < get_adapter_count());

  D3DPRESENT_PARAMETERS pp {};
  pp.BackBufferCount = 1;
  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.hDeviceWindow = window;
  pp.Windowed = !desc.exclusive;
  pp.EnableAutoDepthStencil = TRUE;
  pp.AutoDepthStencilFormat = D3DFMT_D16;
  pp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

  if (desc.exclusive) {
    D3DDISPLAYMODE displayMode {};
    D3D9CALL(mFactory->GetAdapterDisplayMode(desc.adapterIndex, &displayMode));

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  ComPtr<IDirect3DDevice9> d3d9Device;
  D3D9CALL(mFactory->CreateDevice(desc.adapterIndex, D3DDEVTYPE_HAL, window,
                                  D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp,
                                  d3d9Device.GetAddressOf()));

  auto device = std::make_shared<D3D9Device>(std::move(d3d9Device));
  auto context = std::make_shared<D3D9Context>(device);

  return std::make_tuple(std::move(device), std::move(context));
}

auto D3D9Factory::create() -> D3D9FactoryPtr {
  ComPtr<IDirect3D9> factory;
  factory.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!factory) {
    BASALT_LOG_WARN("Direct3D 9 not available");

    return nullptr;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_WARN("D3DX version missmatch");

    return nullptr;
  }

  return std::make_unique<D3D9Factory>(std::move(factory));
}

} // namespace basalt::gfx
