#include "factory.h"

#include "context.h"
#include "device.h"
#include "util.h"

#include <runtime/platform/platform.h>
#include <runtime/shared/config.h>
#include <runtime/shared/log.h>

#include <fmt/format.h>

#include <array>
#include <stdexcept>
#include <string>
#include <utility>

namespace basalt::gfx {

using Microsoft::WRL::ComPtr;

using std::array;
using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::tuple;

namespace {

constexpr array<D3DFORMAT, 4> ALLOWED_DISPLAY_FORMATS {
  D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10};

constexpr array<D3DFORMAT, 6> ALLOWED_BACK_BUFFER_FORMATS {
  D3DFMT_R5G6B5,   D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5,
  D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10};

auto to_surface_format(D3DFORMAT) -> SurfaceFormat;

} // namespace

D3D9Factory::D3D9Factory(ComPtr<IDirect3D9> factory)
  : mFactory {std::move(factory)} {
  D3DADAPTER_IDENTIFIER9 adapterIdentifier;
  D3D9CALL(
    mFactory->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier));

  u16 product = HIWORD(adapterIdentifier.DriverVersion.HighPart);
  u16 version = LOWORD(adapterIdentifier.DriverVersion.HighPart);
  u16 subVersion = HIWORD(adapterIdentifier.DriverVersion.LowPart);
  u16 build = LOWORD(adapterIdentifier.DriverVersion.LowPart);

  mAdapterInfo = AdapterInfo {
    string {adapterIdentifier.Description}, string {adapterIdentifier.Driver},
    fmt::format("{}.{}.{}.{}", product, version, subVersion, build)};

  // query the current (default) adapter mode
  D3DDISPLAYMODE currentAdapterMode {};
  mFactory->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &currentAdapterMode);
  mAdapterInfo.defaultAdapterMode =
    AdapterMode {currentAdapterMode.Width, currentAdapterMode.Height,
                 currentAdapterMode.RefreshRate,
                 to_surface_format(currentAdapterMode.Format)};

  // query adapter modes
  for (const D3DFORMAT format : ALLOWED_DISPLAY_FORMATS) {
    const u32 adapterModeCount =
      mFactory->GetAdapterModeCount(D3DADAPTER_DEFAULT, format);

    mAdapterInfo.adapterModes.reserve(mAdapterInfo.adapterModes.size() +
                                      adapterModeCount);

    for (u32 i = 0; i < adapterModeCount; i++) {
      D3DDISPLAYMODE adapterMode {};
      [[maybe_unused]] const HRESULT hr =
        mFactory->EnumAdapterModes(D3DADAPTER_DEFAULT, format, i, &adapterMode);
      BASALT_ASSERT(SUCCEEDED(hr));

      mAdapterInfo.adapterModes.emplace_back(AdapterMode {
        adapterMode.Width, adapterMode.Height, adapterMode.RefreshRate,
        to_surface_format(adapterMode.Format)});
    }
  }
}

auto D3D9Factory::adapter_info() const noexcept -> const AdapterInfo& {
  return mAdapterInfo;
}

auto D3D9Factory::create_device_and_context(const HWND window) const
  -> tuple<DevicePtr, ContextPtr> {
  D3DPRESENT_PARAMETERS pp {};
  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.hDeviceWindow = window;
  pp.Windowed = TRUE;
  pp.EnableAutoDepthStencil = TRUE;
  pp.AutoDepthStencilFormat = D3DFMT_D16;

  const auto windowMode = platform::get_window_mode();

  // setup exclusive fullscreen
  if (windowMode == WindowMode::FullscreenExclusive) {
    D3DDISPLAYMODE displayMode {};
    D3D9CALL(mFactory->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode));

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.Windowed = FALSE;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  ComPtr<IDirect3DDevice9> d3d9Device {};
  D3D9CALL(mFactory->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                                  D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp,
                                  d3d9Device.GetAddressOf()));

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}({})",
                  mAdapterInfo.displayName, mAdapterInfo.driver,
                  mAdapterInfo.driverVersion);

  auto device = std::make_shared<D3D9Device>(std::move(d3d9Device));

  return std::make_tuple(device, std::make_shared<D3D9Context>(device, pp));
}

auto D3D9Factory::create() -> std::optional<D3D9FactoryPtr> {
  ComPtr<IDirect3D9> factory {};
  factory.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!factory) {
    BASALT_LOG_WARN("Direct3D 9 not available");

    return std::nullopt;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_INFO("D3DX version missmatch");

    return std::nullopt;
  }

  return std::make_unique<D3D9Factory>(std::move(factory));
}

namespace {

auto to_surface_format(const D3DFORMAT format) -> SurfaceFormat {
  switch (format) {
  case D3DFMT_A8R8G8B8:
    return SurfaceFormat::B8G8R8A8;

  case D3DFMT_X8R8G8B8:
    return SurfaceFormat::B8G8R8X8;

  case D3DFMT_R5G6B5:
    return SurfaceFormat::B5G6R5;

  case D3DFMT_X1R5G5B5:
    return SurfaceFormat::B5G5R5X1;

  case D3DFMT_A1R5G5B5:
    return SurfaceFormat::B5G5R5A1;

  case D3DFMT_A2R10G10B10:
    return SurfaceFormat::B10G10R10A2;

  default:
    // TODO: assert or throw (or unreachable)
    BASALT_ASSERT_MSG(false, "unsupported format");
    throw runtime_error {"unsupported format"};
  }
}

} // namespace

} // namespace basalt::gfx
