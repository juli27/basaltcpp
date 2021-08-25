#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/d3d9/context.h>
#include <basalt/gfx/backend/d3d9/device.h>
#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <fmt/format.h>

#include <string>
#include <utility>

namespace basalt::gfx {

using Microsoft::WRL::ComPtr;

using std::string;
using std::tuple;

namespace {

constexpr D3DDEVTYPE DEVICE_TYPE {D3DDEVTYPE_HAL};

// D3D9 allows the following display/adapter formats:
// D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10
// D3D9 allows the following back buffer formats:
// D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5,
// D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10

constexpr auto to_image_format(const D3DFORMAT format) noexcept -> ImageFormat {
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

auto create_info(IDirect3D9& factory) -> Info {
  Info info {};
  const u32 adapterCount {factory.GetAdapterCount()};
  info.adapters.reserve(adapterCount);
  for (u32 adapter = 0; adapter < adapterCount; ++adapter) {
    D3DADAPTER_IDENTIFIER9 adapterIdentifier {};
    D3D9CALL(factory.GetAdapterIdentifier(adapter, 0ul, &adapterIdentifier));

    u16 product = HIWORD(adapterIdentifier.DriverVersion.HighPart);
    u16 version = LOWORD(adapterIdentifier.DriverVersion.HighPart);
    u16 subVersion = HIWORD(adapterIdentifier.DriverVersion.LowPart);
    u16 build = LOWORD(adapterIdentifier.DriverVersion.LowPart);

    string driverInfo {fmt::format("{} ({}.{}.{}.{})", adapterIdentifier.Driver,
                                   product, version, subVersion, build)};

    AdapterCapabilities capabilities {};
    // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
    // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
    // supplied to CreateDevice and should be queried on the device itself.
    D3DCAPS9 d3d9Caps {};
    D3D9CALL(factory.GetDeviceCaps(adapter, DEVICE_TYPE, &d3d9Caps));

    const u32 count {factory.GetAdapterModeCount(adapter, D3DFMT_X8R8G8B8)};
    BASALT_ASSERT(count);

    AdapterModeList adapterModes {};
    adapterModes.reserve(adapterModes.size() + count);

    for (u32 mode = 0; mode < count; ++mode) {
      D3DDISPLAYMODE adapterMode {};
      D3D9CALL(
        factory.EnumAdapterModes(adapter, D3DFMT_X8R8G8B8, mode, &adapterMode));

      adapterModes.emplace_back(AdapterMode {
        adapterMode.Width, adapterMode.Height, adapterMode.RefreshRate});
    }

    info.adapters.emplace_back(AdapterInfo {
      string {adapterIdentifier.Description}, std::move(driverInfo),
      std::move(adapterModes), adapter, capabilities});
  }

  return info;
}

auto verify_minimum_caps(const D3DCAPS9& caps) -> bool {
  if (!(caps.TextureCaps & D3DPTEXTURECAPS_PERSPECTIVE)) {
    return false;
  }

  if (!(caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)) {
    return false;
  }

  if (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
    return false;
  }

  if (!(caps.TextureCaps & D3DPTEXTURECAPS_PROJECTED)) {
    return false;
  }

  if (!(caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)) {
    return false;
  }

  if (!(caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)) {
    return false;
  }

  if (!(caps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP)) {
    return false;
  }

  if (!(caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR)) {
    return false;
  }

  if (!(caps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP)) {
    return false;
  }

  if (!(caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)) {
    return false;
  }

  return true;
}

} // namespace

D3D9Factory::D3D9Factory(Token, ComPtr<IDirect3D9> factory, Info info)
  : mFactory {std::move(factory)}, mInfo {std::move(info)} {
}

auto D3D9Factory::get_current_adapter_mode(const u32 adapterIndex) const
  -> AdapterMode {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  D3DDISPLAYMODE currentAdapterMode;
  mFactory->GetAdapterDisplayMode(adapterIndex, &currentAdapterMode);

  return AdapterMode {currentAdapterMode.Width, currentAdapterMode.Height,
                      currentAdapterMode.RefreshRate};
}

auto D3D9Factory::info() const -> const Info& {
  return mInfo;
}

auto D3D9Factory::get_adapter_count() const -> u32 {
  return mFactory->GetAdapterCount();
}

auto D3D9Factory::get_adapter_monitor(const u32 adapterIndex) const
  -> HMONITOR {
  BASALT_ASSERT(adapterIndex < get_adapter_count());

  return mFactory->GetAdapterMonitor(adapterIndex);
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
  pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  if (desc.exclusive) {
    D3DDISPLAYMODE displayMode {};
    D3D9CALL(mFactory->GetAdapterDisplayMode(desc.adapterIndex, &displayMode));

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  constexpr DWORD flags {D3DCREATE_HARDWARE_VERTEXPROCESSING |
                         D3DCREATE_DISABLE_DRIVER_MANAGEMENT};

  ComPtr<IDirect3DDevice9> d3d9Device;
  D3D9CALL(mFactory->CreateDevice(desc.adapterIndex, DEVICE_TYPE, window, flags,
                                  &pp, &d3d9Device));

  D3DCAPS9 caps {};
  D3D9CALL(d3d9Device->GetDeviceCaps(&caps));

  if (!verify_minimum_caps(caps)) {
    BASALT_LOG_ERROR("minimum device caps not satisfied");
  }

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

  Info info {create_info(*factory.Get())};

  return std::make_unique<D3D9Factory>(Token {}, std::move(factory),
                                       std::move(info));
}

} // namespace basalt::gfx
