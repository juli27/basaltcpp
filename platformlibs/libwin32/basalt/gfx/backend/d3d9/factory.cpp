#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/d3d9/context.h>
#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/types.h>

#include <fmt/format.h>

#include <array>
#include <string>
#include <utility>

namespace basalt::gfx {

using Microsoft::WRL::ComPtr;

using std::array;
using std::string;

namespace {

constexpr D3DDEVTYPE DEVICE_TYPE {D3DDEVTYPE_HAL};

constexpr array<D3DFORMAT, 4> DISPLAY_FORMATS {
  D3DFMT_X1R5G5B5,
  D3DFMT_R5G6B5,
  D3DFMT_X8R8G8B8,
  D3DFMT_A2R10G10B10,
};

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

auto query_info(IDirect3D9& factory) -> Info {
  const u32 adapterCount {factory.GetAdapterCount()};
  AdapterList adapters;
  adapters.reserve(adapterCount);

  for (u32 adapter {0}; adapter < adapterCount; ++adapter) {
    AdapterModeList adapterModes {};
    for (const D3DFORMAT displayFormat : DISPLAY_FORMATS) {
      const u32 modeCount {factory.GetAdapterModeCount(adapter, displayFormat)};
      adapterModes.reserve(adapterModes.size() + modeCount);

      const ImageFormat format {to_image_format(displayFormat)};

      for (u32 modeIndex {0}; modeIndex < modeCount; ++modeIndex) {
        D3DDISPLAYMODE mode {};
        D3D9CALL(
          factory.EnumAdapterModes(adapter, displayFormat, modeIndex, &mode));

        // EnumAdapterModes returns the correct 16-bit format
        adapterModes.emplace_back(AdapterMode {
          mode.Width,
          mode.Height,
          mode.RefreshRate,
          format,
        });
      }
    }

    D3DADAPTER_IDENTIFIER9 adapterIdentifier {};
    D3D9CALL(factory.GetAdapterIdentifier(adapter, 0ul, &adapterIdentifier));

    u16 product {HIWORD(adapterIdentifier.DriverVersion.HighPart)};
    u16 version {LOWORD(adapterIdentifier.DriverVersion.HighPart)};
    u16 subVersion {HIWORD(adapterIdentifier.DriverVersion.LowPart)};
    u16 build {LOWORD(adapterIdentifier.DriverVersion.LowPart)};

    string driverInfo {fmt::format(FMT_STRING("{} ({}.{}.{}.{})"),
                                   adapterIdentifier.Driver, product, version,
                                   subVersion, build)};

    adapters.emplace_back(AdapterInfo {
      string {adapterIdentifier.Description},
      std::move(driverInfo),
      std::move(adapterModes),
      Adapter {adapter},
    });
  }

  return Info {
    std::move(adapters),
    BackendApi::Direct3D9,
  };
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

  if (!(caps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)) {
    return false;
  }

  return true;
}

} // namespace

auto D3D9Factory::create() -> D3D9FactoryPtr {
  InstancePtr instance;
  instance.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!instance || instance->GetAdapterCount() == 0) {
    BASALT_LOG_WARN("Direct3D 9 not available");

    return nullptr;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_WARN("D3DX version missmatch");

    return nullptr;
  }

  return std::make_unique<D3D9Factory>(Token {}, std::move(instance));
}

D3D9Factory::D3D9Factory(Token, InstancePtr instance)
  : mInstance {std::move(instance)}, mInfo {query_info(*mInstance.Get())} {
}

auto D3D9Factory::info() const -> const Info& {
  return mInfo;
}

auto D3D9Factory::get_current_adapter_mode(const Adapter adapter) const
  -> AdapterMode {
  const u32 adapterOrdinal {adapter.value()};

  BASALT_ASSERT(adapterOrdinal < mInfo.adapters.size());

  D3DDISPLAYMODE currentAdapterMode;
  mInstance->GetAdapterDisplayMode(adapterOrdinal, &currentAdapterMode);

  return AdapterMode {currentAdapterMode.Width, currentAdapterMode.Height,
                      currentAdapterMode.RefreshRate};
}

auto D3D9Factory::get_adapter_monitor(const Adapter adapter) const -> HMONITOR {
  const u32 adapterOrdinal {adapter.value()};

  BASALT_ASSERT(adapterOrdinal < mInfo.adapters.size());

  return mInstance->GetAdapterMonitor(adapterOrdinal);
}

auto D3D9Factory::create_device_and_context(
  const HWND window, const DeviceAndContextDesc& desc) const -> ContextPtr {
  const u32 adapterOrdinal {desc.adapter.value()};

  BASALT_ASSERT(adapterOrdinal < mInfo.adapters.size());

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
    D3D9CALL(mInstance->GetAdapterDisplayMode(adapterOrdinal, &displayMode));

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  constexpr DWORD flags {D3DCREATE_HARDWARE_VERTEXPROCESSING |
                         D3DCREATE_DISABLE_DRIVER_MANAGEMENT};

  ComPtr<IDirect3DDevice9> d3d9Device;
  D3D9CALL(mInstance->CreateDevice(adapterOrdinal, DEVICE_TYPE, window, flags,
                                   &pp, &d3d9Device));

  // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
  // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
  // supplied to CreateDevice and should be queried on the device itself.
  D3DCAPS9 caps {};
  D3D9CALL(d3d9Device->GetDeviceCaps(&caps));

  if (!verify_minimum_caps(caps)) {
    BASALT_LOG_ERROR("minimum device caps not satisfied");
  }

  auto device = std::make_unique<D3D9Device>(std::move(d3d9Device));

  return std::make_unique<D3D9Context>(std::move(device));
}

} // namespace basalt::gfx
