#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/d3d9/conversions.h>
#include <basalt/gfx/backend/d3d9/dear_imgui_renderer.h>
#include <basalt/gfx/backend/d3d9/device.h>
#include <basalt/gfx/backend/d3d9/effect.h>
#include <basalt/gfx/backend/d3d9/swap_chain.h>
#include <basalt/gfx/backend/d3d9/texture_3d_support.h>
#include <basalt/gfx/backend/d3d9/x_model_support.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <fmt/format.h>

#include <array>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace basalt::gfx {

using namespace std::literals;
using std::array;
using std::numeric_limits;
using std::string;
using std::string_view;
using std::vector;

namespace {

constexpr auto DEVICE_TYPE = D3DDEVTYPE_HAL;
constexpr auto DEVICE_CREATE_FLAGS =
  D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT;

constexpr auto DISPLAY_FORMATS =
  array{D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10};

constexpr auto BACK_BUFFER_FORMATS =
  array{D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5,
        D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10};

// unsupported formats: D3DFMT_D16_LOCKABLE, D3DFMT_D32, D3DFMT_D15S1,
// D3DFMT_D24X4S4, D3DFMT_D32F_LOCKABLE, D3DFMT_D24FS8
constexpr auto DEPTH_STENCIL_FORMATS =
  array{D3DFMT_D16, D3DFMT_D24X8, D3DFMT_D24S8};

constexpr auto MULTI_SAMPLE_TYPES =
  array{D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_4_SAMPLES,
        D3DMULTISAMPLE_8_SAMPLES};

struct Cap final {
  string_view name;
  DWORD cap{};
};

auto log_missing_cap(Cap const& cap) -> void {
  BASALT_LOG_WARN("D3D9: missing cap: {}", cap.name);
}

auto log_forbidden_cap(Cap const& cap) -> void {
  BASALT_LOG_WARN("D3D9: forbidden cap: {}", cap.name);
}

template <typename MinCaps>
[[nodiscard]] auto verify_caps_present(DWORD const caps, MinCaps const& minCaps)
  -> bool {
  auto allCapsPresent = true;

  for (Cap const& cap : minCaps) {
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

auto verify_minimum_caps(D3DCAPS9 const& caps) -> bool {
  auto allCapsPresent = true;

  static constexpr auto MIN_CAPS3 = array{
    MAKE_CAP(D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD),
  };
  allCapsPresent &= verify_caps_present(caps.Caps3, MIN_CAPS3);

  static constexpr auto MIN_PRESENT_INTERVALS = array{
    MAKE_CAP(D3DPRESENT_INTERVAL_ONE),
  };
  allCapsPresent &=
    verify_caps_present(caps.PresentationIntervals, MIN_PRESENT_INTERVALS);

  static constexpr auto MIN_DEV_CAPS = array{
    MAKE_CAP(D3DDEVCAPS_DRAWPRIMITIVES2),
    MAKE_CAP(D3DDEVCAPS_DRAWPRIMITIVES2EX),
    MAKE_CAP(D3DDEVCAPS_HWTRANSFORMANDLIGHT),
    MAKE_CAP(D3DDEVCAPS_HWRASTERIZATION),
  };
  allCapsPresent &= verify_caps_present(caps.DevCaps, MIN_DEV_CAPS);

  static constexpr auto MIN_PRIMITIVE_MISC_CAPS = array{
    MAKE_CAP(D3DPMISCCAPS_MASKZ),
    MAKE_CAP(D3DPMISCCAPS_CULLNONE),
    MAKE_CAP(D3DPMISCCAPS_CULLCW),
    MAKE_CAP(D3DPMISCCAPS_CULLCCW),
    MAKE_CAP(D3DPMISCCAPS_TSSARGTEMP),
    MAKE_CAP(D3DPMISCCAPS_BLENDOP),
    MAKE_CAP(D3DPMISCCAPS_FOGANDSPECULARALPHA),
  };
  allCapsPresent &=
    verify_caps_present(caps.PrimitiveMiscCaps, MIN_PRIMITIVE_MISC_CAPS);

  static constexpr auto MIN_RASTER_CAPS = array{
    MAKE_CAP(D3DPRASTERCAPS_DITHER),    MAKE_CAP(D3DPRASTERCAPS_ZTEST),
    MAKE_CAP(D3DPRASTERCAPS_FOGVERTEX), MAKE_CAP(D3DPRASTERCAPS_FOGTABLE),
    MAKE_CAP(D3DPRASTERCAPS_FOGRANGE),  MAKE_CAP(D3DPRASTERCAPS_WFOG),
    MAKE_CAP(D3DPRASTERCAPS_ZFOG),
  };
  allCapsPresent &= verify_caps_present(caps.RasterCaps, MIN_RASTER_CAPS);

  static constexpr auto MIN_Z_CMP_CAPS = array{
    MAKE_CAP(D3DPCMPCAPS_NEVER),        MAKE_CAP(D3DPCMPCAPS_LESS),
    MAKE_CAP(D3DPCMPCAPS_EQUAL),        MAKE_CAP(D3DPCMPCAPS_LESSEQUAL),
    MAKE_CAP(D3DPCMPCAPS_GREATER),      MAKE_CAP(D3DPCMPCAPS_NOTEQUAL),
    MAKE_CAP(D3DPCMPCAPS_GREATEREQUAL), MAKE_CAP(D3DPCMPCAPS_ALWAYS),
  };
  allCapsPresent &= verify_caps_present(caps.ZCmpCaps, MIN_Z_CMP_CAPS);

  static constexpr auto MIN_SRC_BLEND_CAPS = array{
    MAKE_CAP(D3DPBLENDCAPS_ZERO),
    MAKE_CAP(D3DPBLENDCAPS_ONE),
    MAKE_CAP(D3DPBLENDCAPS_SRCCOLOR),
    MAKE_CAP(D3DPBLENDCAPS_INVSRCCOLOR),
    MAKE_CAP(D3DPBLENDCAPS_SRCALPHA),
    MAKE_CAP(D3DPBLENDCAPS_INVSRCALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_DESTALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_INVDESTALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_DESTCOLOR),
    // MAKE_CAP(D3DPBLENDCAPS_INVDESTCOLOR),
    // MAKE_CAP(D3DPBLENDCAPS_SRCALPHASAT),
    MAKE_CAP(D3DPBLENDCAPS_BLENDFACTOR),
  };
  allCapsPresent &= verify_caps_present(caps.SrcBlendCaps, MIN_SRC_BLEND_CAPS);

  static constexpr auto MIN_DEST_BLEND_CAPS = array{
    MAKE_CAP(D3DPBLENDCAPS_ZERO),
    MAKE_CAP(D3DPBLENDCAPS_ONE),
    MAKE_CAP(D3DPBLENDCAPS_SRCCOLOR),
    MAKE_CAP(D3DPBLENDCAPS_INVSRCCOLOR),
    MAKE_CAP(D3DPBLENDCAPS_SRCALPHA),
    MAKE_CAP(D3DPBLENDCAPS_INVSRCALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_DESTALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_INVDESTALPHA),
    // MAKE_CAP(D3DPBLENDCAPS_DESTCOLOR),
    // MAKE_CAP(D3DPBLENDCAPS_INVDESTCOLOR),
    // MAKE_CAP(D3DPBLENDCAPS_SRCALPHASAT),
    MAKE_CAP(D3DPBLENDCAPS_BLENDFACTOR),
  };
  allCapsPresent &=
    verify_caps_present(caps.DestBlendCaps, MIN_DEST_BLEND_CAPS);

  static constexpr auto MIN_ALPHA_CMP_CAPS = array{
    MAKE_CAP(D3DPCMPCAPS_NEVER),        MAKE_CAP(D3DPCMPCAPS_LESS),
    MAKE_CAP(D3DPCMPCAPS_EQUAL),        MAKE_CAP(D3DPCMPCAPS_LESSEQUAL),
    MAKE_CAP(D3DPCMPCAPS_GREATER),      MAKE_CAP(D3DPCMPCAPS_NOTEQUAL),
    MAKE_CAP(D3DPCMPCAPS_GREATEREQUAL), MAKE_CAP(D3DPCMPCAPS_ALWAYS),
  };
  allCapsPresent &= verify_caps_present(caps.AlphaCmpCaps, MIN_ALPHA_CMP_CAPS);

  static constexpr auto MIN_SHADE_CAPS = array{
    MAKE_CAP(D3DPSHADECAPS_COLORGOURAUDRGB),
    MAKE_CAP(D3DPSHADECAPS_SPECULARGOURAUDRGB),
    MAKE_CAP(D3DPSHADECAPS_ALPHAGOURAUDBLEND),
    MAKE_CAP(D3DPSHADECAPS_FOGGOURAUD),
  };
  allCapsPresent &= verify_caps_present(caps.ShadeCaps, MIN_SHADE_CAPS);

  static constexpr auto MIN_TEXTURE_CAPS = array{
    MAKE_CAP(D3DPTEXTURECAPS_PERSPECTIVE),
    MAKE_CAP(D3DPTEXTURECAPS_ALPHA),
    MAKE_CAP(D3DPTEXTURECAPS_PROJECTED),
    MAKE_CAP(D3DPTEXTURECAPS_CUBEMAP),
    MAKE_CAP(D3DPTEXTURECAPS_VOLUMEMAP),
    MAKE_CAP(D3DPTEXTURECAPS_MIPMAP),
    MAKE_CAP(D3DPTEXTURECAPS_MIPVOLUMEMAP),
    MAKE_CAP(D3DPTEXTURECAPS_MIPCUBEMAP),
  };
  allCapsPresent &= verify_caps_present(caps.TextureCaps, MIN_TEXTURE_CAPS);

  static constexpr auto FORBIDDEN_TEXTURE_CAPS = array{
    MAKE_CAP(D3DPTEXTURECAPS_SQUAREONLY),
  };
  for (auto const& cap : FORBIDDEN_TEXTURE_CAPS) {
    if (caps.TextureCaps & cap.cap) {
      allCapsPresent = false;
      log_forbidden_cap(cap);
    }
  }

  static constexpr auto MIN_TEXTURE_FILTER_CAPS = array{
    MAKE_CAP(D3DPTFILTERCAPS_MINFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MINFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MIPFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MIPFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MAGFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MAGFLINEAR),
  };
  allCapsPresent &=
    verify_caps_present(caps.TextureFilterCaps, MIN_TEXTURE_FILTER_CAPS);

  static constexpr auto MIN_CUBE_TEXTURE_FILTER_CAPS = array{
    MAKE_CAP(D3DPTFILTERCAPS_MINFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MINFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MIPFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MIPFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MAGFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MAGFLINEAR),
  };
  allCapsPresent &= verify_caps_present(caps.CubeTextureFilterCaps,
                                        MIN_CUBE_TEXTURE_FILTER_CAPS);

  static constexpr auto MIN_VOLUME_TEXTURE_FILTER_CAPS = array{
    MAKE_CAP(D3DPTFILTERCAPS_MINFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MINFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MIPFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MIPFLINEAR),
    MAKE_CAP(D3DPTFILTERCAPS_MAGFPOINT), MAKE_CAP(D3DPTFILTERCAPS_MAGFLINEAR),
  };
  allCapsPresent &= verify_caps_present(caps.VolumeTextureFilterCaps,
                                        MIN_VOLUME_TEXTURE_FILTER_CAPS);

  static constexpr auto MIN_TEXTURE_ADDRESS_CAPS = array{
    MAKE_CAP(D3DPTADDRESSCAPS_WRAP),
    MAKE_CAP(D3DPTADDRESSCAPS_MIRROR),
    MAKE_CAP(D3DPTADDRESSCAPS_CLAMP),
    MAKE_CAP(D3DPTADDRESSCAPS_INDEPENDENTUV),
  };
  allCapsPresent &=
    verify_caps_present(caps.TextureAddressCaps, MIN_TEXTURE_ADDRESS_CAPS);

  static constexpr auto MIN_VOLUME_TEXTURE_ADDRESS_CAPS = array{
    MAKE_CAP(D3DPTADDRESSCAPS_WRAP),
    MAKE_CAP(D3DPTADDRESSCAPS_MIRROR),
    MAKE_CAP(D3DPTADDRESSCAPS_CLAMP),
    MAKE_CAP(D3DPTADDRESSCAPS_INDEPENDENTUV),
  };
  allCapsPresent &= verify_caps_present(caps.VolumeTextureAddressCaps,
                                        MIN_VOLUME_TEXTURE_ADDRESS_CAPS);

  static constexpr auto MIN_STENCIL_CAPS = array{
    MAKE_CAP(D3DSTENCILCAPS_KEEP),     MAKE_CAP(D3DSTENCILCAPS_ZERO),
    MAKE_CAP(D3DSTENCILCAPS_REPLACE),  MAKE_CAP(D3DSTENCILCAPS_INCRSAT),
    MAKE_CAP(D3DSTENCILCAPS_DECRSAT),  MAKE_CAP(D3DSTENCILCAPS_INVERT),
    MAKE_CAP(D3DSTENCILCAPS_INCR),     MAKE_CAP(D3DSTENCILCAPS_DECR),
    MAKE_CAP(D3DSTENCILCAPS_TWOSIDED),
  };
  allCapsPresent &= verify_caps_present(caps.StencilCaps, MIN_STENCIL_CAPS);

  {
    constexpr auto minNumTexCoords = u8{1};
    auto const numTexCoords =
      static_cast<WORD>(caps.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK);
    if (numTexCoords < minNumTexCoords) {
      BASALT_LOG_WARN("D3D9: only {} sets of texture coordinates supported",
                      minNumTexCoords);
      allCapsPresent = false;
    }
  }

  static constexpr auto MIN_TEXTURE_OP_CAPS = array{
    MAKE_CAP(D3DTEXOPCAPS_DISABLE),
    MAKE_CAP(D3DTEXOPCAPS_SELECTARG1),
    MAKE_CAP(D3DTEXOPCAPS_MODULATE),
    MAKE_CAP(D3DTEXOPCAPS_MODULATE2X),
    MAKE_CAP(D3DTEXOPCAPS_MODULATE4X),
    MAKE_CAP(D3DTEXOPCAPS_ADD),
    MAKE_CAP(D3DTEXOPCAPS_ADDSIGNED),
    MAKE_CAP(D3DTEXOPCAPS_ADDSIGNED2X),
    MAKE_CAP(D3DTEXOPCAPS_SUBTRACT),
    MAKE_CAP(D3DTEXOPCAPS_ADDSMOOTH),
    MAKE_CAP(D3DTEXOPCAPS_BLENDDIFFUSEALPHA),
    MAKE_CAP(D3DTEXOPCAPS_BLENDTEXTUREALPHA),
    MAKE_CAP(D3DTEXOPCAPS_BLENDFACTORALPHA),
    MAKE_CAP(D3DTEXOPCAPS_BLENDTEXTUREALPHAPM),
    MAKE_CAP(D3DTEXOPCAPS_BLENDCURRENTALPHA),
    MAKE_CAP(D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR),
    MAKE_CAP(D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA),
    MAKE_CAP(D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR),
    MAKE_CAP(D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA),
    MAKE_CAP(D3DTEXOPCAPS_BUMPENVMAP),
    MAKE_CAP(D3DTEXOPCAPS_BUMPENVMAPLUMINANCE),
    MAKE_CAP(D3DTEXOPCAPS_DOTPRODUCT3),
    MAKE_CAP(D3DTEXOPCAPS_MULTIPLYADD),
    MAKE_CAP(D3DTEXOPCAPS_LERP),
  };
  allCapsPresent &=
    verify_caps_present(caps.TextureOpCaps, MIN_TEXTURE_OP_CAPS);

  static constexpr auto MIN_VERTEX_PROCESSING_CAPS = array{
    MAKE_CAP(D3DVTXPCAPS_DIRECTIONALLIGHTS),
    MAKE_CAP(D3DVTXPCAPS_LOCALVIEWER),
    MAKE_CAP(D3DVTXPCAPS_MATERIALSOURCE7),
    MAKE_CAP(D3DVTXPCAPS_POSITIONALLIGHTS),
  };
  allCapsPresent &=
    verify_caps_present(caps.VertexProcessingCaps, MIN_VERTEX_PROCESSING_CAPS);

  static constexpr auto MIN_DEV_CAPS2 = array{
    MAKE_CAP(D3DDEVCAPS2_STREAMOFFSET),
  };
  allCapsPresent &= verify_caps_present(caps.DevCaps2, MIN_DEV_CAPS2);

  return allCapsPresent;
}

#undef MAKE_CAP

auto enum_display_modes(IDirect3D9& instance, UINT const adapter,
                        D3DFORMAT const displayFormat) -> DisplayModeList {
  auto displayModes = DisplayModeList{};

  auto const modeCount = instance.GetAdapterModeCount(adapter, displayFormat);
  displayModes.reserve(modeCount);

  for (auto modeIndex = UINT{0}; modeIndex < modeCount; ++modeIndex) {
    auto mode = D3DDISPLAYMODE{};
    D3D9CHECK(
      instance.EnumAdapterModes(adapter, displayFormat, modeIndex, &mode));

    displayModes.emplace_back(DisplayMode{
      mode.Width,
      mode.Height,
      mode.RefreshRate,
    });
  }

  return displayModes;
}

auto enum_depth_stencil_formats(IDirect3D9& instance, UINT const adapter,
                                D3DFORMAT const displayFormat)
  -> vector<ImageFormat> {
  auto depthStencilFormats = vector<ImageFormat>{};

  for (auto const depthStencilFormat : DEPTH_STENCIL_FORMATS) {
    auto const hr = instance.CheckDeviceFormat(
      adapter, DEVICE_TYPE, displayFormat, D3DUSAGE_DEPTHSTENCIL,
      D3DRTYPE_SURFACE, depthStencilFormat);

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    D3D9CHECK(hr);

    if (SUCCEEDED(hr)) {
      depthStencilFormats.emplace_back(to_image_format(depthStencilFormat));
    }
  }

  return depthStencilFormats;
}

auto enum_multi_sample_counts(IDirect3D9& instance, UINT const adapter,
                              D3DFORMAT const format, BOOL const windowed)
  -> MultiSampleCounts {
  MultiSampleCounts counts{};

  for (auto const type : MULTI_SAMPLE_TYPES) {
    auto const hr = instance.CheckDeviceMultiSampleType(
      adapter, DEVICE_TYPE, format, windowed, type, nullptr);

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    D3D9CHECK(hr);
    if (FAILED(hr)) {
      continue;
    }

    counts.set(to_multi_sample_count(type));
  }

  return counts;
}

auto enum_back_buffer_formats(IDirect3D9& instance, UINT const adapter,
                              D3DFORMAT const displayFormat,
                              BOOL const windowed) -> vector<BackBufferFormat> {
  auto backBufferFormats = vector<BackBufferFormat>{};
  backBufferFormats.reserve(BACK_BUFFER_FORMATS.size());

  for (auto const backBufferFormat : BACK_BUFFER_FORMATS) {
    auto hr = instance.CheckDeviceType(adapter, DEVICE_TYPE, displayFormat,
                                       backBufferFormat, windowed);

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }

    // other errors codes are not expected
    D3D9CHECK(hr);
    if (FAILED(hr)) {
      continue;
    }

    auto const depthStencilFormats =
      enum_depth_stencil_formats(instance, adapter, displayFormat);

    auto multiSampleCounts =
      enum_multi_sample_counts(instance, adapter, backBufferFormat, windowed);

    for (auto const depthStencilFormat : depthStencilFormats) {
      auto const format = to_d3d(depthStencilFormat);

      hr = instance.CheckDepthStencilMatch(adapter, DEVICE_TYPE, displayFormat,
                                           backBufferFormat, format);

      if (hr == D3DERR_NOTAVAILABLE) {
        continue;
      }

      // other errors codes are not expected
      D3D9CHECK(hr);
      if (FAILED(hr)) {
        continue;
      }

      multiSampleCounts &=
        enum_multi_sample_counts(instance, adapter, format, windowed);

      if (!multiSampleCounts.has(MultiSampleCount::One)) {
        continue;
      }

      backBufferFormats.emplace_back(BackBufferFormat{
        to_image_format(backBufferFormat),
        depthStencilFormat,
        multiSampleCounts,
      });
    }
  }

  backBufferFormats.shrink_to_fit();

  return backBufferFormats;
}

auto enum_suitable_adapter_modes(IDirect3D9& instance, UINT const adapter)
  -> AdapterModeList {
  auto adapterModes = AdapterModeList{};
  adapterModes.reserve(DISPLAY_FORMATS.size());

  for (auto const displayFormat : DISPLAY_FORMATS) {
    auto backBufferFormats =
      enum_back_buffer_formats(instance, adapter, displayFormat, FALSE);

    if (backBufferFormats.empty()) {
      continue;
    }

    auto displayModes = enum_display_modes(instance, adapter, displayFormat);

    if (displayModes.empty()) {
      continue;
    }

    adapterModes.emplace_back(AdapterModes{
      std::move(backBufferFormats),
      std::move(displayModes),
      to_image_format(displayFormat),
    });
  }

  adapterModes.shrink_to_fit();

  return adapterModes;
}

auto enum_suitable_adapters(IDirect3D9& instance) -> AdapterList {
  auto const adapterCount = instance.GetAdapterCount();

  BASALT_LOG_INFO("d3d9: testing {} adapters for suitability", adapterCount);

  auto adapters = AdapterList{};
  adapters.reserve(adapterCount);

  for (auto adapter = UINT{0}; adapter < adapterCount; ++adapter) {
    // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
    // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
    // supplied to CreateDevice and should be queried on the device itself.
    auto caps = D3DCAPS9{};
    D3D9CHECK(instance.GetDeviceCaps(adapter, DEVICE_TYPE, &caps));

    if (!verify_minimum_caps(caps)) {
      BASALT_LOG_INFO("d3d9: adapter {} minimum device caps not present",
                      adapter);

      continue;
    }

    auto adapterModes = enum_suitable_adapter_modes(instance, adapter);

    if (adapterModes.empty()) {
      BASALT_LOG_INFO("d3d9: adapter {} has no useable adapter mode", adapter);

      continue;
    }

    auto adapterIdentifier = D3DADAPTER_IDENTIFIER9{};
    D3D9CHECK(instance.GetAdapterIdentifier(adapter, 0ul, &adapterIdentifier));

    auto const product = HIWORD(adapterIdentifier.DriverVersion.HighPart);
    auto const version = LOWORD(adapterIdentifier.DriverVersion.HighPart);
    auto const subVersion = HIWORD(adapterIdentifier.DriverVersion.LowPart);
    auto const build = LOWORD(adapterIdentifier.DriverVersion.LowPart);

    auto driverInfo =
      fmt::format(FMT_STRING("{} ({}.{}.{}.{})"), adapterIdentifier.Driver,
                  product, version, subVersion, build);

    auto currentMode = D3DDISPLAYMODE{};
    D3D9CHECK(instance.GetAdapterDisplayMode(adapter, &currentMode));

    auto backBufferFormats =
      enum_back_buffer_formats(instance, adapter, currentMode.Format, TRUE);

    if (backBufferFormats.empty()) {
      BASALT_LOG_INFO("d3d9: adapter {} has no back buffer formats", adapter);

      continue;
    }

    adapters.emplace_back(AdapterInfo{
      string{adapterIdentifier.Description},
      std::move(driverInfo),
      std::move(adapterModes),
      std::move(backBufferFormats),
      DisplayMode{
        currentMode.Width,
        currentMode.Height,
        currentMode.RefreshRate,
      },
      to_image_format(currentMode.Format),
      Adapter{adapter},
    });
  }

  return adapters;
}

auto get_device_caps(IDirect3DDevice9& device) -> DeviceCaps {
  DeviceCaps caps;

  auto d3d9Caps = D3DCAPS9{};
  D3D9CHECK(device.GetDeviceCaps(&d3d9Caps));

  caps.maxVertexBufferSizeInBytes = numeric_limits<UINT>::max();
  caps.maxIndexBufferSizeInBytes = numeric_limits<UINT>::max();

  if (d3d9Caps.MaxVertexIndex > 0xffff) {
    caps.supportedIndexTypes.set(IndexType::U32);
  }

  caps.maxLights = d3d9Caps.MaxActiveLights;
  caps.maxTextureBlendStages = d3d9Caps.MaxTextureBlendStages;
  caps.maxBoundSampledTextures = d3d9Caps.MaxSimultaneousTextures;

  caps.samplerClampToBorder =
    d3d9Caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER &&
    d3d9Caps.VolumeTextureAddressCaps & D3DPTADDRESSCAPS_BORDER;
  caps.samplerCustomBorderColor = caps.samplerClampToBorder;
  caps.samplerMirrorOnceClampToEdge =
    d3d9Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE &&
    d3d9Caps.VolumeTextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE;

  caps.samplerMinFilterAnisotropic =
    d3d9Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC;
  caps.samplerMagFilterAnisotropic =
    d3d9Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC;
  caps.samplerCubeMinFilterAnisotropic =
    d3d9Caps.CubeTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC;
  caps.samplerCubeMagFilterAnisotropic =
    d3d9Caps.CubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC;
  caps.sampler3DMinFilterAnisotropic =
    d3d9Caps.VolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC;
  caps.sampler3DMagFilterAnisotropic =
    d3d9Caps.VolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC;
  caps.samplerMaxAnisotropy = saturated_cast<u8>(d3d9Caps.MaxAnisotropy);
  caps.perTextureStageConstant =
    d3d9Caps.PrimitiveMiscCaps & D3DPMISCCAPS_PERSTAGECONSTANT;
  caps.supportedColorOps = TextureOps{TextureOp::Replace,
                                      TextureOp::Modulate,
                                      TextureOp::Modulate2X,
                                      TextureOp::Modulate4X,
                                      TextureOp::Add,
                                      TextureOp::AddSigned,
                                      TextureOp::AddSigned2X,
                                      TextureOp::Subtract,
                                      TextureOp::AddSmooth,
                                      TextureOp::BlendDiffuseAlpha,
                                      TextureOp::BlendTextureAlpha,
                                      TextureOp::BlendFactorAlpha,
                                      TextureOp::BlendCurrentAlpha,
                                      TextureOp::BlendTextureAlphaPm,
                                      TextureOp::ModulateAlphaAddColor,
                                      TextureOp::ModulateColorAddAlpha,
                                      TextureOp::ModulateInvAlphaAddColor,
                                      TextureOp::ModulateInvColorAddAlpha,
                                      TextureOp::BumpEnvMap,
                                      TextureOp::BumpEnvMapLuminance,
                                      TextureOp::DotProduct3,
                                      TextureOp::MultiplyAdd,
                                      TextureOp::Interpolate};
  caps.supportedAlphaOps = TextureOps{TextureOp::Replace,
                                      TextureOp::Modulate,
                                      TextureOp::Modulate2X,
                                      TextureOp::Modulate4X,
                                      TextureOp::Add,
                                      TextureOp::AddSigned,
                                      TextureOp::AddSigned2X,
                                      TextureOp::Subtract,
                                      TextureOp::AddSmooth,
                                      TextureOp::BlendDiffuseAlpha,
                                      TextureOp::BlendTextureAlpha,
                                      TextureOp::BlendFactorAlpha,
                                      TextureOp::BlendCurrentAlpha,
                                      TextureOp::BlendTextureAlphaPm,
                                      TextureOp::MultiplyAdd,
                                      TextureOp::Interpolate};

  if (d3d9Caps.TextureOpCaps & D3DTEXOPCAPS_PREMODULATE) {
    caps.supportedColorOps.set(TextureOp::PreModulate);
    // TODO: can TextureOp::PreModulate be an alpha op too?
  }

  return caps;
}

} // namespace

auto D3D9Factory::create() -> D3D9FactoryPtr {
  auto instance = IDirect3D9Ptr{};
  instance.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!instance) {
    BASALT_LOG_WARN("d3d9: not available");

    return nullptr;
  }

  auto adapters = enum_suitable_adapters(*instance.Get());

  if (adapters.empty()) {
    BASALT_LOG_WARN("d3d9: no suitable adapter");

    return nullptr;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_WARN("D3DX version missmatch");

    return nullptr;
  }

  return std::make_unique<D3D9Factory>(std::move(instance),
                                       std::move(adapters));
}

auto D3D9Factory::get_adapter_monitor(Adapter const adapter) const -> HMONITOR {
  auto const adapterOrdinal = adapter.value();
  BASALT_ASSERT(adapterOrdinal < mInstance->GetAdapterCount());

  return mInstance->GetAdapterMonitor(adapterOrdinal);
}

auto D3D9Factory::adapters() const -> AdapterList const& {
  return mAdapters;
}

D3D9Factory::D3D9Factory(IDirect3D9Ptr instance, AdapterList adapters)
  : mInstance{std::move(instance)}
  , mAdapters{std::move(adapters)} {
}

auto D3D9Factory::do_create_device_and_swap_chain(
  HWND const window, DeviceAndSwapChainDesc const& desc) const
  -> DeviceAndSwapChain {
  auto const adapterOrdinal = desc.adapter.value();
  BASALT_ASSERT(adapterOrdinal < mInstance->GetAdapterCount());

  auto pp = D3DPRESENT_PARAMETERS{};
  pp.BackBufferFormat = to_d3d(desc.renderTargetFormat);
  pp.BackBufferCount = 1;
  pp.MultiSampleType = to_d3d(desc.sampleCount);
  pp.MultiSampleQuality = 0;
  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.hDeviceWindow = window;
  pp.Windowed = !desc.exclusive;
  pp.EnableAutoDepthStencil = desc.depthStencilFormat != ImageFormat::Unknown;
  pp.AutoDepthStencilFormat = to_d3d(desc.depthStencilFormat);
  pp.Flags =
    pp.EnableAutoDepthStencil ? D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL : 0;
  pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  if (desc.exclusive) {
    pp.BackBufferWidth = desc.exclusiveDisplayMode.width;
    pp.BackBufferHeight = desc.exclusiveDisplayMode.height;
    pp.FullScreen_RefreshRateInHz = desc.exclusiveDisplayMode.refreshRate;
  }

  auto d3d9Device = IDirect3DDevice9Ptr{};
  D3D9CHECK(mInstance->CreateDevice(adapterOrdinal, DEVICE_TYPE, window,
                                    DEVICE_CREATE_FLAGS, &pp, &d3d9Device));

  // TODO: verify the five caps which differ?
  
  auto const deviceCaps = get_device_caps(*d3d9Device.Get());
  auto device = D3D9Device::create(d3d9Device, deviceCaps);
  auto deviceExtensions = ext::DeviceExtensions{
    {ext::DeviceExtensionId::DearImGuiRenderer,
     ext::D3D9ImGuiRenderer::create(d3d9Device)},
    {ext::DeviceExtensionId::Effects, ext::D3D9XEffects::create(device)},
    {ext::DeviceExtensionId::Texture3DSupport,
     ext::D3D9Texture3DSupport::create(device)},
    {ext::DeviceExtensionId::XModelSupport,
     ext::D3D9XModelSupport::create(d3d9Device)},
  };
  device->set_extensions(deviceExtensions);

  auto implicitSwapChain = IDirect3DSwapChain9Ptr{};
  D3D9CHECK(d3d9Device->GetSwapChain(0, &implicitSwapChain));

  auto swapChain = D3D9SwapChain::create(device, std::move(implicitSwapChain));

  return DeviceAndSwapChain{std::move(device), std::move(deviceExtensions),
                            std::move(swapChain)};
}

} // namespace basalt::gfx
