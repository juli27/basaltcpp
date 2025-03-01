#include "factory.h"

#include "conversions.h"
#include "d3d9_error.h"
#include "dear_imgui_renderer.h"
#include "device.h"
#include "effect.h"
#include "swap_chain.h"
#include "texture_3d_support.h"
#include "x_model_support.h"

#include <basalt/api/base/functional.h>
#include <basalt/api/base/log.h>
#include <basalt/api/base/types.h>

#include <fmt/format.h>

#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace basalt::gfx {

using namespace std::literals;
using std::array;
using std::nullopt;
using std::numeric_limits;
using std::optional;
using std::string;
using std::string_view;
using std::vector;

namespace {

constexpr auto DEVICE_TYPE = D3DDEVTYPE_HAL;

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
[[nodiscard]]
auto verify_caps_present(DWORD const caps, MinCaps const& minCaps) -> bool {
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
                        D3DFORMAT const displayFormat) -> DisplayModes {
  auto displayModes = DisplayModes{};

  auto const modeCount = instance.GetAdapterModeCount(adapter, displayFormat);
  displayModes.reserve(modeCount);

  for (auto i = UINT{}; i < modeCount; i++) {
    auto mode = D3DDISPLAYMODE{};
    D3D9CHECK(instance.EnumAdapterModes(adapter, displayFormat, i, &mode));

    displayModes.emplace_back(
      DisplayMode{mode.Width, mode.Height, mode.RefreshRate});
  }

  return displayModes;
}

auto enum_depth_stencil_formats(IDirect3D9& instance, UINT const adapter,
                                D3DFORMAT const displayFormat)
  -> vector<D3DFORMAT> {
  // unsupported formats: D3DFMT_D32F_LOCKABLE, D3DFMT_D16_LOCKABLE
  constexpr auto depthStencilFormatCandidates =
    array{D3DFMT_D32,   D3DFMT_D24FS8, D3DFMT_D24S8, D3DFMT_D24X4S4,
          D3DFMT_D24X8, D3DFMT_D16,    D3DFMT_D15S1};

  auto depthStencilFormats = vector<D3DFORMAT>{};

  for (auto const depthStencilFormat : depthStencilFormatCandidates) {
    auto const hr = instance.CheckDeviceFormat(
      adapter, DEVICE_TYPE, displayFormat, D3DUSAGE_DEPTHSTENCIL,
      D3DRTYPE_SURFACE, depthStencilFormat);

    if (hr == D3DERR_NOTAVAILABLE) {
      continue;
    }
    D3D9CHECK(hr);
    if (FAILED(hr)) {
      continue;
    }

    depthStencilFormats.emplace_back(depthStencilFormat);
  }

  return depthStencilFormats;
}

auto enum_multi_sample_counts(IDirect3D9& instance, UINT const adapter,
                              D3DFORMAT const format,
                              BOOL const windowed) -> MultiSampleCounts {
  // TODO: D3DMULTISAMPLE_NONMASKABLE
  constexpr auto multisamplingTypes =
    array{D3DMULTISAMPLE_NONE,       D3DMULTISAMPLE_2_SAMPLES,
          D3DMULTISAMPLE_3_SAMPLES,  D3DMULTISAMPLE_4_SAMPLES,
          D3DMULTISAMPLE_5_SAMPLES,  D3DMULTISAMPLE_6_SAMPLES,
          D3DMULTISAMPLE_7_SAMPLES,  D3DMULTISAMPLE_8_SAMPLES,
          D3DMULTISAMPLE_9_SAMPLES,  D3DMULTISAMPLE_10_SAMPLES,
          D3DMULTISAMPLE_11_SAMPLES, D3DMULTISAMPLE_12_SAMPLES,
          D3DMULTISAMPLE_13_SAMPLES, D3DMULTISAMPLE_14_SAMPLES,
          D3DMULTISAMPLE_15_SAMPLES, D3DMULTISAMPLE_16_SAMPLES};
  auto counts = MultiSampleCounts{};

  for (auto const type : multisamplingTypes) {
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
                              BOOL const windowed) -> BackBufferFormats {
  constexpr auto backBufferFormatsCandidates =
    array{D3DFMT_A2R10G10B10, D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8,
          D3DFMT_R5G6B5,      D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5};
  auto backBufferFormats = BackBufferFormats{};
  backBufferFormats.reserve(backBufferFormatsCandidates.size());

  for (auto const backBufferFormat : backBufferFormatsCandidates) {
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

    auto multiSampleCounts =
      enum_multi_sample_counts(instance, adapter, backBufferFormat, windowed);

    auto const depthStencilFormats =
      enum_depth_stencil_formats(instance, adapter, displayFormat);

    for (auto const depthStencilFormat : depthStencilFormats) {
      hr =
        instance.CheckDepthStencilMatch(adapter, DEVICE_TYPE, displayFormat,
                                        backBufferFormat, depthStencilFormat);
      if (hr == D3DERR_NOTAVAILABLE) {
        continue;
      }
      // other errors codes are not expected
      D3D9CHECK(hr);
      if (FAILED(hr)) {
        continue;
      }

      multiSampleCounts &= enum_multi_sample_counts(
        instance, adapter, depthStencilFormat, windowed);

      backBufferFormats.emplace_back(BackBufferFormat{
        to_image_format(backBufferFormat).value(),
        to_image_format(depthStencilFormat).value(), multiSampleCounts});
    }
  }

  backBufferFormats.shrink_to_fit();

  return backBufferFormats;
}

// filter out adapters with insufficient caps
auto enum_suitable_adapters(IDirect3D9& instance) -> vector<UINT> {
  auto const adapterCount = instance.GetAdapterCount();
  auto adapters = vector<UINT>{};
  adapters.reserve(adapterCount);

  for (auto i = UINT{}; i < adapterCount; i++) {
    // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
    // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
    // supplied to CreateDevice and should be queried on the device itself.
    auto caps = D3DCAPS9{};
    D3D9CHECK(instance.GetDeviceCaps(i, DEVICE_TYPE, &caps));
    if (!verify_minimum_caps(caps)) {
      BASALT_LOG_INFO("d3d9: adapter {} minimum device caps not present", i);

      continue;
    }

    adapters.push_back(i);
  }

  BASALT_LOG_INFO("d3d9: {} of {} adapters are suitable", adapters.size(),
                  adapterCount);

  return adapters;
}

auto to_device_caps(D3DCAPS9 const& d3d9Caps) -> DeviceCaps {
  auto caps = DeviceCaps{};

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

auto get_device_caps(IDirect3DDevice9& device) -> DeviceCaps {
  auto d3d9Caps = D3DCAPS9{};
  D3D9CHECK(device.GetDeviceCaps(&d3d9Caps));

  return to_device_caps(d3d9Caps);
}

} // namespace

auto D3D9Factory::create() -> optional<D3D9FactoryPtr> {
  auto instance = IDirect3D9Ptr{};
  instance.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!instance) {
    BASALT_LOG_WARN("d3d9: IDirect3D9 creation failed");

    return nullopt;
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_WARN("d3d9: D3DX version check failed");

    return nullopt;
  }

  auto suitableAdapters = enum_suitable_adapters(*instance.Get());
  if (suitableAdapters.empty()) {
    BASALT_LOG_WARN("d3d9: no suitable adapter");

    return nullopt;
  }

  return std::make_unique<D3D9Factory>(std::move(instance),
                                       std::move(suitableAdapters));
}

D3D9Factory::D3D9Factory(IDirect3D9Ptr instance,
                         std::vector<UINT> suitableAdapters)
  : mInstance{std::move(instance)}
  , mSuitableAdapters{std::move(suitableAdapters)} {
}

auto D3D9Factory::adapter_count() const -> u32 {
  return static_cast<u32>(mSuitableAdapters.size());
}

auto D3D9Factory::get_adapter_identifier(u32 const adapterIndex) const
  -> AdapterIdentifier {
  auto const adapter = mSuitableAdapters.at(adapterIndex);

  auto identifier = D3DADAPTER_IDENTIFIER9{};
  D3D9CHECK(mInstance->GetAdapterIdentifier(adapter, 0, &identifier));

  auto id = [&] {
    auto const& guid = identifier.DeviceIdentifier;

    return fmt::format(FMT_STRING("{:08X}-{:04X}-{:04X}-{:02X}{:02X}-"
                                  "{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}"),
                       guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
                       guid.Data4[1], guid.Data4[2], guid.Data4[3],
                       guid.Data4[4], guid.Data4[5], guid.Data4[6],
                       guid.Data4[7]);
  }();

  auto displayName = string{identifier.Description};
  auto driverInfo = [&] {
    auto const product = HIWORD(identifier.DriverVersion.HighPart);
    auto const version = LOWORD(identifier.DriverVersion.HighPart);
    auto const subVersion = HIWORD(identifier.DriverVersion.LowPart);
    auto const build = LOWORD(identifier.DriverVersion.LowPart);

    return fmt::format(FMT_STRING("{} v{}.{}.{}.{}"), identifier.Driver,
                       product, version, subVersion, build);
  }();

  auto pciId = PciId{LOWORD(identifier.VendorId), LOWORD(identifier.DeviceId),
                     LOBYTE(identifier.Revision), LOWORD(identifier.SubSysId),
                     HIWORD(identifier.SubSysId)};

  return AdapterIdentifier{std::move(id), std::move(displayName),
                           std::move(driverInfo), pciId};
}

auto D3D9Factory::get_adapter_device_caps(u32 adapterIndex) const
  -> DeviceCaps {
  auto const adapter = mSuitableAdapters.at(adapterIndex);

  // VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes,
  // MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex depend on parameters
  // supplied to CreateDevice and should be queried on the device itself
  auto caps = D3DCAPS9{};
  D3D9CHECK(mInstance->GetDeviceCaps(adapter, DEVICE_TYPE, &caps));

  return to_device_caps(caps);
}

auto D3D9Factory::get_adapter_shared_mode_info(u32 const adapterIndex) const
  -> AdapterSharedModeInfo {
  auto const adapter = mSuitableAdapters.at(adapterIndex);

  auto mode = D3DDISPLAYMODE{};
  D3D9CHECK(mInstance->GetAdapterDisplayMode(adapter, &mode));

  auto backBufferFormats =
    enum_back_buffer_formats(*mInstance.Get(), adapter, mode.Format, TRUE);

  return AdapterSharedModeInfo{
    std::move(backBufferFormats),
    DisplayMode{mode.Width, mode.Height, mode.RefreshRate},
    to_image_format(mode.Format).value()};
}

auto D3D9Factory::enum_adapter_exclusive_mode_infos(
  u32 const adapterIndex) const -> AdapterExclusiveModeInfos {
  auto const adapter = mSuitableAdapters.at(adapterIndex);

  constexpr auto displayFormats =
    array{D3DFMT_A2R10G10B10, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5};

  auto modes = AdapterExclusiveModeInfos{};
  modes.reserve(displayFormats.size());

  for (auto const displayFormat : displayFormats) {
    auto displayModes =
      enum_display_modes(*mInstance.Get(), adapter, displayFormat);
    if (displayModes.empty()) {
      continue;
    }

    auto backBufferFormats =
      enum_back_buffer_formats(*mInstance.Get(), adapter, displayFormat, FALSE);
    if (backBufferFormats.empty()) {
      continue;
    }

    modes.emplace_back(AdapterExclusiveModeInfo{
      std::move(backBufferFormats),
      std::move(displayModes),
      to_image_format(displayFormat).value(),
    });
  }

  return modes;
}

auto D3D9Factory::do_create_device_and_swap_chain(
  HWND const window, u32 const adapter,
  SwapChain::Info const& info) const -> DeviceAndSwapChain {
  auto const adapterOrdinal = mSuitableAdapters.at(adapter);

  auto pp = D3D9SwapChain::to_present_parameters(info);
  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
  pp.hDeviceWindow = window;

  constexpr auto createFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
  auto d3d9Device = IDirect3DDevice9Ptr{};
  D3D9CHECK(mInstance->CreateDevice(adapterOrdinal, DEVICE_TYPE, window,
                                    createFlags, &pp, &d3d9Device));

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
