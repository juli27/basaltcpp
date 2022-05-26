#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/d3d9/context.h>
#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <fmt/format.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace basalt::gfx {

using Microsoft::WRL::ComPtr;

using namespace std::literals;
using std::array;
using std::string;
using std::string_view;
using std::vector;

namespace {

constexpr D3DDEVTYPE DEVICE_TYPE {D3DDEVTYPE_HAL};
constexpr DWORD DEVICE_CREATE_FLAGS {D3DCREATE_HARDWARE_VERTEXPROCESSING |
                                     D3DCREATE_DISABLE_DRIVER_MANAGEMENT};

constexpr array<D3DFORMAT, 4> DISPLAY_FORMATS {
  D3DFMT_X1R5G5B5,
  D3DFMT_R5G6B5,
  D3DFMT_X8R8G8B8,
  D3DFMT_A2R10G10B10,
};

constexpr array<D3DFORMAT, 6> BACK_BUFFER_FORMATS {
  D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5,
  D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10,
};

// unsupported formats: D3DFMT_D16_LOCKABLE, D3DFMT_D32, D3DFMT_D15S1,
// D3DFMT_D24X4S4, D3DFMT_D32F_LOCKABLE, D3DFMT_D24FS8
constexpr array<D3DFORMAT, 3> DEPTH_STENCIL_FORMATS {
  D3DFMT_D16,
  D3DFMT_D24X8,
  D3DFMT_D24S8,
};

constexpr array<D3DMULTISAMPLE_TYPE, 4> MULTI_SAMPLE_TYPES {
  D3DMULTISAMPLE_NONE,
  D3DMULTISAMPLE_2_SAMPLES,
  D3DMULTISAMPLE_4_SAMPLES,
  D3DMULTISAMPLE_8_SAMPLES,
};

struct Cap final {
  string_view name;
  DWORD cap {};
};

auto log_missing_cap(const Cap& cap) -> void {
  BASALT_LOG_WARN("D3D9: missing cap: {}", cap.name);
}

auto log_forbidden_cap(const Cap& cap) -> void {
  BASALT_LOG_WARN("D3D9: forbidden cap: {}", cap.name);
}

template <typename MinCaps>
[[nodiscard]] auto verify_caps_present(const DWORD caps, const MinCaps& minCaps)
  -> bool {
  bool allCapsPresent {true};

  for (const Cap& cap : minCaps) {
    if (!(caps & cap.cap)) {
      allCapsPresent = false;
      log_missing_cap(cap);
    }
  }

  return allCapsPresent;
}

#define MAKE_CAP(cap)                                                          \
  Cap {                                                                        \
#cap##sv, cap,                                                             \
  }

auto verify_minimum_caps(const D3DCAPS9& caps) -> bool {
  bool allCapsPresent {true};

  static constexpr array<Cap, 4> MIN_TEXTURE_CAPS {
    MAKE_CAP(D3DPTEXTURECAPS_PERSPECTIVE),
    MAKE_CAP(D3DPTEXTURECAPS_ALPHA),
    MAKE_CAP(D3DPTEXTURECAPS_PROJECTED),
    MAKE_CAP(D3DPTEXTURECAPS_MIPMAP),
  };

  allCapsPresent &= verify_caps_present(caps.TextureCaps, MIN_TEXTURE_CAPS);

  static constexpr array<Cap, 1> FORBIDDEN_TEXTURE_CAPS {
    MAKE_CAP(D3DPTEXTURECAPS_SQUAREONLY),
  };

  for (const Cap& cap : FORBIDDEN_TEXTURE_CAPS) {
    if (caps.TextureCaps & cap.cap) {
      allCapsPresent = false;
      log_forbidden_cap(cap);
    }
  }

  static constexpr array<Cap, 6> MIN_TEXTURE_FILTER_CAPS {
    MAKE_CAP(D3DPTFILTERCAPS_MINFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MINFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MIPFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MIPFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MAGFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MAGFLINEAR),
  };

  allCapsPresent &=
    verify_caps_present(caps.TextureFilterCaps, MIN_TEXTURE_FILTER_CAPS);

  static constexpr array<Cap, 4> MIN_TEXTURE_ADDRESS_CAPS {
    MAKE_CAP(D3DPTADDRESSCAPS_WRAP),
    MAKE_CAP(D3DPTADDRESSCAPS_MIRROR),
    MAKE_CAP(D3DPTADDRESSCAPS_CLAMP),
    MAKE_CAP(D3DPTADDRESSCAPS_INDEPENDENTUV),
  };

  allCapsPresent &=
    verify_caps_present(caps.TextureAddressCaps, MIN_TEXTURE_ADDRESS_CAPS);

  static constexpr array<Cap, 1> MIN_DEV_CAPS2 {
    MAKE_CAP(D3DDEVCAPS2_STREAMOFFSET),
  };

  allCapsPresent &= verify_caps_present(caps.DevCaps2, MIN_DEV_CAPS2);

  return allCapsPresent;
}

#undef MAKE_CAP

auto enum_display_modes(IDirect3D9& instance, const UINT adapter,
                        const D3DFORMAT displayFormat) -> DisplayModeList {
  DisplayModeList displayModes;

  const UINT modeCount {instance.GetAdapterModeCount(adapter, displayFormat)};
  displayModes.reserve(modeCount);

  for (UINT modeIndex {0}; modeIndex < modeCount; ++modeIndex) {
    D3DDISPLAYMODE mode {};
    D3D9CALL(
      instance.EnumAdapterModes(adapter, displayFormat, modeIndex, &mode));

    displayModes.emplace_back(DisplayMode {
      mode.Width,
      mode.Height,
      mode.RefreshRate,
    });
  }

  return displayModes;
}

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

  case D3DFMT_D24S8:
    return ImageFormat::D24S8;

  case D3DFMT_D24X8:
    return ImageFormat::D24X8;

  case D3DFMT_D16:
    return ImageFormat::D16;

  case D3DFMT_UNKNOWN:
  case D3DFMT_D16_LOCKABLE:
  case D3DFMT_D32:
  case D3DFMT_D15S1:
  case D3DFMT_D24X4S4:
  case D3DFMT_D32F_LOCKABLE:
  case D3DFMT_D24FS8:
  default:
    return ImageFormat::Unknown;
  }
}

auto to_d3d(const ImageFormat format) -> D3DFORMAT {
  static constexpr EnumArray<ImageFormat, D3DFORMAT, 10> TO_D3D {
    {ImageFormat::Unknown, D3DFMT_UNKNOWN},
    {ImageFormat::B5G6R5, D3DFMT_R5G6B5},
    {ImageFormat::B5G5R5X1, D3DFMT_X1R5G5B5},
    {ImageFormat::B5G5R5A1, D3DFMT_A1R5G5B5},
    {ImageFormat::B8G8R8X8, D3DFMT_X8R8G8B8},
    {ImageFormat::B8G8R8A8, D3DFMT_A8R8G8B8},
    {ImageFormat::B10G10R10A2, D3DFMT_A2R10G10B10},
    {ImageFormat::D16, D3DFMT_D16},
    {ImageFormat::D24X8, D3DFMT_D24X8},
    {ImageFormat::D24S8, D3DFMT_D24S8},
  };
  static_assert(TO_D3D.size() == IMAGE_FORMAT_COUNT);

  return TO_D3D[format];
}

auto to_multi_sample_count(const D3DMULTISAMPLE_TYPE type) -> MultiSampleCount {
  switch (type) {
  case D3DMULTISAMPLE_NONE:
    return MultiSampleCount::One;
  case D3DMULTISAMPLE_2_SAMPLES:
    return MultiSampleCount::Two;
  case D3DMULTISAMPLE_4_SAMPLES:
    return MultiSampleCount::Four;
  case D3DMULTISAMPLE_8_SAMPLES:
    return MultiSampleCount::Eight;
  default:
    BASALT_CRASH("unknown multisample type");
  }
}

auto enum_depth_stencil_formats(IDirect3D9& instance, const UINT adapter,
                                const D3DFORMAT displayFormat)
  -> vector<ImageFormat> {
  vector<ImageFormat> depthStencilFormats;

  for (const D3DFORMAT depthStencilFormat : DEPTH_STENCIL_FORMATS) {
    const HRESULT hr {instance.CheckDeviceFormat(
      adapter, DEVICE_TYPE, displayFormat, D3DUSAGE_DEPTHSTENCIL,
      D3DRTYPE_SURFACE, depthStencilFormat)};

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    D3D9CALL(hr);

    if (SUCCEEDED(hr)) {
      depthStencilFormats.emplace_back(to_image_format(depthStencilFormat));
    }
  }

  return depthStencilFormats;
}

auto enum_multi_sample_counts(IDirect3D9& instance, const UINT adapter,
                              const D3DFORMAT format, const BOOL windowed)
  -> MultiSampleCounts {
  MultiSampleCounts counts {};

  for (const auto type : MULTI_SAMPLE_TYPES) {
    const HRESULT hr {instance.CheckDeviceMultiSampleType(
      adapter, DEVICE_TYPE, format, windowed, type, nullptr)};

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    D3D9CALL(hr);
    if (FAILED(hr)) {
      continue;
    }

    counts.set(to_multi_sample_count(type));
  }

  return counts;
}

auto enum_back_buffer_formats(IDirect3D9& instance, const UINT adapter,
                              const D3DFORMAT displayFormat,
                              const BOOL windowed) -> vector<BackBufferFormat> {
  vector<BackBufferFormat> backBufferFormats;
  backBufferFormats.reserve(BACK_BUFFER_FORMATS.size());

  for (const D3DFORMAT backBufferFormat : BACK_BUFFER_FORMATS) {
    HRESULT hr {instance.CheckDeviceType(adapter, DEVICE_TYPE, displayFormat,
                                         backBufferFormat, windowed)};

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    // other errors codes are not expected
    D3D9CALL(hr);
    if (FAILED(hr)) {
      continue;
    }

    const auto depthStencilFormats {
      enum_depth_stencil_formats(instance, adapter, displayFormat)};

    MultiSampleCounts multiSampleCounts {
      enum_multi_sample_counts(instance, adapter, backBufferFormat, windowed)};

    for (const ImageFormat depthStencilFormat : depthStencilFormats) {
      const D3DFORMAT format {to_d3d(depthStencilFormat)};

      hr = instance.CheckDepthStencilMatch(adapter, DEVICE_TYPE, displayFormat,
                                           backBufferFormat, format);

      if (hr == D3DERR_NOTAVAILABLE) {
        continue;
      }

      // other errors codes are not expected
      D3D9CALL(hr);
      if (FAILED(hr)) {
        continue;
      }

      multiSampleCounts &=
        enum_multi_sample_counts(instance, adapter, format, windowed);

      if (!multiSampleCounts.has(MultiSampleCount::One)) {
        continue;
      }

      backBufferFormats.emplace_back(BackBufferFormat {
        to_image_format(backBufferFormat),
        depthStencilFormat,
        multiSampleCounts,
      });
    }
  }

  backBufferFormats.shrink_to_fit();

  return backBufferFormats;
}

auto enum_suitable_adapter_modes(IDirect3D9& instance, const UINT adapter)
  -> AdapterModeList {
  AdapterModeList adapterModes;
  adapterModes.reserve(DISPLAY_FORMATS.size());

  for (const D3DFORMAT displayFormat : DISPLAY_FORMATS) {
    auto backBufferFormats {
      enum_back_buffer_formats(instance, adapter, displayFormat, FALSE)};

    if (backBufferFormats.empty()) {
      continue;
    }

    DisplayModeList displayModes {
      enum_display_modes(instance, adapter, displayFormat)};

    if (displayModes.empty()) {
      continue;
    }

    adapterModes.emplace_back(AdapterModes {
      std::move(backBufferFormats),
      std::move(displayModes),
      to_image_format(displayFormat),
    });
  }

  adapterModes.shrink_to_fit();

  return adapterModes;
}

auto enum_suitable_adapters(IDirect3D9& instance) -> AdapterList {
  const UINT adapterCount {instance.GetAdapterCount()};

  BASALT_LOG_INFO("d3d9: testing {} adapters for suitability", adapterCount);

  AdapterList adapters;
  adapters.reserve(adapterCount);

  for (UINT adapter {0}; adapter < adapterCount; ++adapter) {
    // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
    // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
    // supplied to CreateDevice and should be queried on the device itself.
    D3DCAPS9 caps {};
    D3D9CALL(instance.GetDeviceCaps(adapter, DEVICE_TYPE, &caps));

    if (!verify_minimum_caps(caps)) {
      BASALT_LOG_INFO("d3d9: adapter {} minimum device caps not present",
                      adapter);

      continue;
    }

    AdapterModeList adapterModes {
      enum_suitable_adapter_modes(instance, adapter)};

    if (adapterModes.empty()) {
      BASALT_LOG_INFO("d3d9: adapter {} has no useable adapter mode", adapter);

      continue;
    }

    D3DADAPTER_IDENTIFIER9 adapterIdentifier {};
    D3D9CALL(instance.GetAdapterIdentifier(adapter, 0ul, &adapterIdentifier));

    const u16 product {HIWORD(adapterIdentifier.DriverVersion.HighPart)};
    const u16 version {LOWORD(adapterIdentifier.DriverVersion.HighPart)};
    const u16 subVersion {HIWORD(adapterIdentifier.DriverVersion.LowPart)};
    const u16 build {LOWORD(adapterIdentifier.DriverVersion.LowPart)};

    string driverInfo {fmt::format(FMT_STRING("{} ({}.{}.{}.{})"),
                                   adapterIdentifier.Driver, product, version,
                                   subVersion, build)};

    D3DDISPLAYMODE currentMode {};
    D3D9CALL(instance.GetAdapterDisplayMode(adapter, &currentMode));

    auto backBufferFormats {
      enum_back_buffer_formats(instance, adapter, currentMode.Format, TRUE)};

    if (backBufferFormats.empty()) {
      BASALT_LOG_INFO("d3d9: adapter {} has no back buffer formats", adapter);

      continue;
    }

    adapters.emplace_back(AdapterInfo {
      string {adapterIdentifier.Description},
      std::move(driverInfo),
      std::move(adapterModes),
      std::move(backBufferFormats),
      DisplayMode {
        currentMode.Width,
        currentMode.Height,
        currentMode.RefreshRate,
      },
      to_image_format(currentMode.Format),
      Adapter {adapter},
    });
  }

  return adapters;
}

} // namespace

auto D3D9Factory::create() -> D3D9FactoryPtr {
  InstancePtr instance;
  instance.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!instance) {
    BASALT_LOG_WARN("d3d9: not available");

    return nullptr;
  }

  AdapterList adapters {enum_suitable_adapters(*instance.Get())};

  if (adapters.empty()) {
    BASALT_LOG_WARN("d3d9: no suitable adapter");

    return nullptr;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_WARN("D3DX version missmatch");

    return nullptr;
  }

  return std::make_unique<D3D9Factory>(Token {}, std::move(instance),
                                       Info {
                                         std::move(adapters),
                                         BackendApi::Direct3D9,
                                       });
}

D3D9Factory::D3D9Factory(Token, InstancePtr instance, Info info)
  : mInstance {std::move(instance)}, mInfo {std::move(info)} {
}

auto D3D9Factory::info() const -> const Info& {
  return mInfo;
}

auto D3D9Factory::get_adapter_monitor(const Adapter adapter) const -> HMONITOR {
  const UINT adapterOrdinal {adapter.value()};
  BASALT_ASSERT(adapterOrdinal < mInstance->GetAdapterCount());

  return mInstance->GetAdapterMonitor(adapterOrdinal);
}

auto D3D9Factory::create_device_and_context(
  const HWND window, const DeviceAndContextDesc& desc) const -> ContextPtr {
  const UINT adapterOrdinal {desc.adapter.value()};
  BASALT_ASSERT(adapterOrdinal < mInstance->GetAdapterCount());

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

  ComPtr<IDirect3DDevice9> d3d9Device;
  D3D9CALL(mInstance->CreateDevice(adapterOrdinal, DEVICE_TYPE, window,
                                   DEVICE_CREATE_FLAGS, &pp, &d3d9Device));

  // TODO: verify the five caps which differ?

  auto device = std::make_unique<D3D9Device>(std::move(d3d9Device));

  return std::make_unique<D3D9Context>(std::move(device));
}

} // namespace basalt::gfx
