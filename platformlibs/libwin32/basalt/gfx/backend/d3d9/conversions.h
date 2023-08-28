#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/utils.h>

#include <algorithm>

namespace basalt::gfx {

constexpr auto to_device_status(const HRESULT hr) -> DeviceStatus {
  if (SUCCEEDED(hr)) {
    return DeviceStatus::Ok;
  }

  switch (hr) {
  case D3DERR_DEVICELOST:
    return DeviceStatus::DeviceLost;

  case D3DERR_DEVICENOTRESET:
    return DeviceStatus::ResetNeeded;

  default:
    return DeviceStatus::Error;
  }
}

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

constexpr auto to_d3d_color_value(const Color& color) noexcept
  -> D3DCOLORVALUE {
  return D3DCOLORVALUE {color.r(), color.g(), color.b(), color.a()};
}

constexpr auto to_color(const D3DCOLORVALUE& color) -> Color {
  return Color::from_non_linear(color.r, color.g, color.b, color.a);
}

constexpr auto to_d3d(const Matrix4x4f32& mat) noexcept -> D3DMATRIX {
  // clang-format off
  return D3DMATRIX {mat.m11, mat.m12, mat.m13, mat.m14,
                    mat.m21, mat.m22, mat.m23, mat.m24,
                    mat.m31, mat.m32, mat.m33, mat.m34,
                    mat.m41, mat.m42, mat.m43, mat.m44};
  // clang-format on
}

constexpr auto to_d3d(const Vector3f32& vec) noexcept -> D3DVECTOR {
  return D3DVECTOR {vec.x(), vec.y(), vec.z()};
}

inline auto to_d3d(const BlendFactor factor) -> D3DBLEND {
  static constexpr EnumArray<BlendFactor, D3DBLEND, 8> TO_D3D {
    {BlendFactor::Zero, D3DBLEND_ZERO},
    {BlendFactor::One, D3DBLEND_ONE},
    {BlendFactor::SrcColor, D3DBLEND_SRCCOLOR},
    {BlendFactor::OneMinusSrcColor, D3DBLEND_INVSRCCOLOR},
    {BlendFactor::SrcAlpha, D3DBLEND_SRCALPHA},
    {BlendFactor::OneMinusSrcAlpha, D3DBLEND_INVSRCALPHA},
    {BlendFactor::Constant, D3DBLEND_BLENDFACTOR},
    {BlendFactor::OneMinusConstant, D3DBLEND_INVBLENDFACTOR},
  };
  static_assert(BLEND_FACTOR_COUNT == TO_D3D.size());

  return TO_D3D[factor];
}

inline auto to_d3d(const BlendOp op) -> D3DBLENDOP {
  static constexpr EnumArray<BlendOp, D3DBLENDOP, 3> TO_D3D {
    {BlendOp::Add, D3DBLENDOP_ADD},
    {BlendOp::Subtract, D3DBLENDOP_SUBTRACT},
    {BlendOp::ReverseSubtract, D3DBLENDOP_REVSUBTRACT},
  };
  static_assert(BLEND_OP_COUNT == TO_D3D.size());

  return TO_D3D[op];
}

inline auto to_d3d(const CullMode mode) -> D3DCULL {
  static constexpr EnumArray<CullMode, D3DCULL, 3> TO_D3D {
    {CullMode::None, D3DCULL_NONE},
    {CullMode::Clockwise, D3DCULL_CW},
    {CullMode::CounterClockwise, D3DCULL_CCW},
  };
  static_assert(CULL_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const FillMode mode) -> D3DFILLMODE {
  static constexpr EnumArray<FillMode, D3DFILLMODE, 3> TO_D3D {
    {FillMode::Point, D3DFILL_POINT},
    {FillMode::Wireframe, D3DFILL_WIREFRAME},
    {FillMode::Solid, D3DFILL_SOLID},
  };
  static_assert(FILL_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const FogMode mode) -> D3DFOGMODE {
  static constexpr EnumArray<FogMode, D3DFOGMODE, 4> TO_D3D {
    {FogMode::None, D3DFOG_NONE},
    {FogMode::Linear, D3DFOG_LINEAR},
    {FogMode::Exponential, D3DFOG_EXP},
    {FogMode::ExponentialSquared, D3DFOG_EXP2},
  };
  static_assert(FOG_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const ImageFormat format) -> D3DFORMAT {
  static constexpr EnumArray<ImageFormat, D3DFORMAT, 11> TO_D3D {
    {ImageFormat::Unknown, D3DFMT_UNKNOWN},
    {ImageFormat::B5G6R5, D3DFMT_R5G6B5},
    {ImageFormat::B5G5R5X1, D3DFMT_X1R5G5B5},
    {ImageFormat::B5G5R5A1, D3DFMT_A1R5G5B5},
    {ImageFormat::B8G8R8X8, D3DFMT_X8R8G8B8},
    {ImageFormat::B8G8R8A8, D3DFMT_A8R8G8B8},
    {ImageFormat::B10G10R10A2, D3DFMT_A2R10G10B10},
    {ImageFormat::U8V8, D3DFMT_V8U8},
    {ImageFormat::D16, D3DFMT_D16},
    {ImageFormat::D24X8, D3DFMT_D24X8},
    {ImageFormat::D24S8, D3DFMT_D24S8},
  };
  static_assert(TO_D3D.size() == IMAGE_FORMAT_COUNT);

  return TO_D3D[format];
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

  case D3DFMT_V8U8:
    return ImageFormat::U8V8;

  case D3DFMT_D24S8:
    return ImageFormat::D24S8;

  case D3DFMT_D24X8:
    return ImageFormat::D24X8;

  case D3DFMT_D16:
    return ImageFormat::D16;

  case D3DFMT_UNKNOWN:
  default:
    break;
  }

  return ImageFormat::Unknown;
}

inline auto to_d3d(const IndexType type) -> D3DFORMAT {
  static constexpr EnumArray<IndexType, D3DFORMAT, 2> TO_D3D {
    {IndexType::U16, D3DFMT_INDEX16},
    {IndexType::U32, D3DFMT_INDEX32},
  };
  static_assert(TO_D3D.size() == INDEX_TYPE_COUNT);

  return TO_D3D[type];
}

inline auto to_d3d(const MaterialColorSource mcs) -> D3DMATERIALCOLORSOURCE {
  static constexpr EnumArray<MaterialColorSource, D3DMATERIALCOLORSOURCE, 3>
    TO_D3D {{MaterialColorSource::DiffuseVertexColor, D3DMCS_COLOR1},
            {MaterialColorSource::SpecularVertexColor, D3DMCS_COLOR2},
            {MaterialColorSource::Material, D3DMCS_MATERIAL}};
  static_assert(TO_D3D.size() == MATERIAL_COLOR_SOURCE_COUNT);

  return TO_D3D[mcs];
}

inline auto to_d3d(const MultiSampleCount sampleCount) -> D3DMULTISAMPLE_TYPE {
  static constexpr EnumArray<MultiSampleCount, D3DMULTISAMPLE_TYPE, 4> TO_D3D {
    {MultiSampleCount::One, D3DMULTISAMPLE_NONE},
    {MultiSampleCount::Two, D3DMULTISAMPLE_2_SAMPLES},
    {MultiSampleCount::Four, D3DMULTISAMPLE_4_SAMPLES},
    {MultiSampleCount::Eight, D3DMULTISAMPLE_8_SAMPLES},
  };
  static_assert(TO_D3D.size() == MULTI_SAMPLE_COUNT_COUNT);

  return TO_D3D[sampleCount];
}

inline auto to_multi_sample_count(const D3DMULTISAMPLE_TYPE sampleType)
  -> MultiSampleCount {
  switch (sampleType) {
  case D3DMULTISAMPLE_NONE:
    return MultiSampleCount::One;
  case D3DMULTISAMPLE_2_SAMPLES:
    return MultiSampleCount::Two;
  case D3DMULTISAMPLE_4_SAMPLES:
    return MultiSampleCount::Four;
  case D3DMULTISAMPLE_8_SAMPLES:
    return MultiSampleCount::Eight;
  default:
    break;
  }

  BASALT_CRASH("unknown multisample type");
}

inline auto to_d3d(const PrimitiveType primitiveType) -> D3DPRIMITIVETYPE {
  static constexpr EnumArray<PrimitiveType, D3DPRIMITIVETYPE, 6> TO_D3D {
    {PrimitiveType::PointList, D3DPT_POINTLIST},
    {PrimitiveType::LineList, D3DPT_LINELIST},
    {PrimitiveType::LineStrip, D3DPT_LINESTRIP},
    {PrimitiveType::TriangleList, D3DPT_TRIANGLELIST},
    {PrimitiveType::TriangleStrip, D3DPT_TRIANGLESTRIP},
    {PrimitiveType::TriangleFan, D3DPT_TRIANGLEFAN},
  };
  static_assert(PRIMITIVE_TYPE_COUNT == TO_D3D.size());

  return TO_D3D[primitiveType];
}

inline auto to_d3d(const ShadeMode mode) -> D3DSHADEMODE {
  static constexpr EnumArray<ShadeMode, D3DSHADEMODE, 2> TO_D3D {
    {ShadeMode::Flat, D3DSHADE_FLAT},
    {ShadeMode::Gouraud, D3DSHADE_GOURAUD},
  };
  static_assert(SHADE_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const StencilOp op) -> D3DSTENCILOP {
  static constexpr EnumArray<StencilOp, D3DSTENCILOP, 8> TO_D3D {
    {StencilOp::Keep, D3DSTENCILOP_KEEP},
    {StencilOp::Zero, D3DSTENCILOP_ZERO},
    {StencilOp::Replace, D3DSTENCILOP_REPLACE},
    {StencilOp::Invert, D3DSTENCILOP_INVERT},
    {StencilOp::IncrementClamp, D3DSTENCILOP_INCRSAT},
    {StencilOp::DecrementClamp, D3DSTENCILOP_DECRSAT},
    {StencilOp::IncrementWrap, D3DSTENCILOP_INCR},
    {StencilOp::DecrementWrap, D3DSTENCILOP_DECR},
  };
  static_assert(STENCIL_OP_COUNT == TO_D3D.size());

  return TO_D3D[op];
}

inline auto to_d3d(const TestPassCond function) -> D3DCMPFUNC {
  static constexpr EnumArray<TestPassCond, D3DCMPFUNC, 8> TO_D3D {
    {TestPassCond::Never, D3DCMP_NEVER},
    {TestPassCond::IfEqual, D3DCMP_EQUAL},
    {TestPassCond::IfNotEqual, D3DCMP_NOTEQUAL},
    {TestPassCond::IfLess, D3DCMP_LESS},
    {TestPassCond::IfLessEqual, D3DCMP_LESSEQUAL},
    {TestPassCond::IfGreater, D3DCMP_GREATER},
    {TestPassCond::IfGreaterEqual, D3DCMP_GREATEREQUAL},
    {TestPassCond::Always, D3DCMP_ALWAYS},
  };
  static_assert(TEST_PASS_COND_COUNT == TO_D3D.size());

  return TO_D3D[function];
}

inline auto to_d3d(const TextureAddressMode mode) -> D3DTEXTUREADDRESS {
  static constexpr EnumArray<TextureAddressMode, D3DTEXTUREADDRESS, 5> TO_D3D {
    {TextureAddressMode::Repeat, D3DTADDRESS_WRAP},
    {TextureAddressMode::Mirror, D3DTADDRESS_MIRROR},
    {TextureAddressMode::ClampToEdge, D3DTADDRESS_CLAMP},
    {TextureAddressMode::ClampToBorder, D3DTADDRESS_BORDER},
    {TextureAddressMode::MirrorOnceClampToEdge, D3DTADDRESS_MIRRORONCE},
  };
  static_assert(TEXTURE_ADDRESS_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const TextureCoordinateSrc src) -> DWORD {
  static constexpr EnumArray<TextureCoordinateSrc, DWORD, 4> TO_D3D {
    {TextureCoordinateSrc::Vertex, D3DTSS_TCI_PASSTHRU},
    {TextureCoordinateSrc::PositionInViewSpace, D3DTSS_TCI_CAMERASPACEPOSITION},
    {TextureCoordinateSrc::NormalInViewSpace, D3DTSS_TCI_CAMERASPACENORMAL},
    {TextureCoordinateSrc::ReflectionVectorInViewSpace,
     D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR},
  };
  static_assert(TEXTURE_COORDINATE_SOURCE_COUNT == TO_D3D.size());

  return TO_D3D[src];
}

inline auto to_d3d(const TextureFilter filter) -> D3DTEXTUREFILTERTYPE {
  static constexpr EnumArray<TextureFilter, D3DTEXTUREFILTERTYPE, 3> TO_D3D {
    {TextureFilter::Point, D3DTEXF_POINT},
    {TextureFilter::Bilinear, D3DTEXF_LINEAR},
    {TextureFilter::Anisotropic, D3DTEXF_ANISOTROPIC},
  };
  static_assert(TEXTURE_FILTER_COUNT == TO_D3D.size());

  return TO_D3D[filter];
}

inline auto to_d3d(const TextureMipFilter filter) -> D3DTEXTUREFILTERTYPE {
  static constexpr EnumArray<TextureMipFilter, D3DTEXTUREFILTERTYPE, 3> TO_D3D {
    {TextureMipFilter::None, D3DTEXF_NONE},
    {TextureMipFilter::Point, D3DTEXF_POINT},
    {TextureMipFilter::Linear, D3DTEXF_LINEAR},
  };
  static_assert(TEXTURE_MIP_FILTER_COUNT == TO_D3D.size());

  return TO_D3D[filter];
}

inline auto to_d3d(const TextureOp op) -> D3DTEXTUREOP {
  static constexpr EnumArray<TextureOp, D3DTEXTUREOP, 24> TO_D3D {
    {TextureOp::Replace, D3DTOP_SELECTARG1},
    {TextureOp::Modulate, D3DTOP_MODULATE},
    {TextureOp::Modulate2X, D3DTOP_MODULATE2X},
    {TextureOp::Modulate4X, D3DTOP_MODULATE4X},
    {TextureOp::Add, D3DTOP_ADD},
    {TextureOp::AddSigned, D3DTOP_ADDSIGNED},
    {TextureOp::AddSigned2X, D3DTOP_ADDSIGNED2X},
    {TextureOp::Subtract, D3DTOP_SUBTRACT},
    {TextureOp::AddSmooth, D3DTOP_ADDSMOOTH},
    {TextureOp::BlendDiffuseAlpha, D3DTOP_BLENDDIFFUSEALPHA},
    {TextureOp::BlendTextureAlpha, D3DTOP_BLENDTEXTUREALPHA},
    {TextureOp::BlendFactorAlpha, D3DTOP_BLENDFACTORALPHA},
    {TextureOp::BlendCurrentAlpha, D3DTOP_BLENDCURRENTALPHA},
    {TextureOp::BlendTextureAlphaPm, D3DTOP_BLENDTEXTUREALPHAPM},
    {TextureOp::PreModulate, D3DTOP_PREMODULATE},
    {TextureOp::ModulateAlphaAddColor, D3DTOP_MODULATEALPHA_ADDCOLOR},
    {TextureOp::ModulateColorAddAlpha, D3DTOP_MODULATECOLOR_ADDALPHA},
    {TextureOp::ModulateInvAlphaAddColor, D3DTOP_MODULATEINVALPHA_ADDCOLOR},
    {TextureOp::ModulateInvColorAddAlpha, D3DTOP_MODULATEINVCOLOR_ADDALPHA},
    {TextureOp::BumpEnvMap, D3DTOP_BUMPENVMAP},
    {TextureOp::BumpEnvMapLuminance, D3DTOP_BUMPENVMAPLUMINANCE},
    {TextureOp::DotProduct3, D3DTOP_DOTPRODUCT3},
    {TextureOp::MultiplyAdd, D3DTOP_MULTIPLYADD},
    {TextureOp::Interpolate, D3DTOP_LERP},
  };
  static_assert(TEXTURE_OP_COUNT == TO_D3D.size());

  return TO_D3D[op];
}

inline auto to_d3d(const TextureStageSrc src) -> DWORD {
  static constexpr EnumArray<TextureStageSrc, DWORD, 7> TO_D3D {
    {TextureStageSrc::Current, D3DTA_CURRENT},
    {TextureStageSrc::Diffuse, D3DTA_DIFFUSE},
    {TextureStageSrc::Specular, D3DTA_SPECULAR},
    {TextureStageSrc::SampledTexture, D3DTA_TEXTURE},
    {TextureStageSrc::TextureFactor, D3DTA_TFACTOR},
    {TextureStageSrc::Temporary, D3DTA_TEMP},
    {TextureStageSrc::StageConstant, D3DTA_CONSTANT},
  };
  static_assert(TEXTURE_STAGE_SRC_COUNT == TO_D3D.size());

  return TO_D3D[src];
}

inline auto to_d3d(const TextureStageSrcMod modifier) -> DWORD {
  static constexpr EnumArray<TextureStageSrcMod, DWORD, 3> TO_D3D {
    {TextureStageSrcMod::None, 0},
    {TextureStageSrcMod::Complement, D3DTA_COMPLEMENT},
    {TextureStageSrcMod::AlphaReplicate, D3DTA_ALPHAREPLICATE},
  };
  static_assert(TEXTURE_STAGE_SRC_MOD_COUNT == TO_D3D.size());

  return TO_D3D[modifier];
}

inline auto to_d3d(const TextureStageArgument& arg) -> DWORD {
  return to_d3d(arg.src) | to_d3d(arg.modifier);
}

inline auto to_d3d(const TextureStageDestination dest) -> DWORD {
  static constexpr EnumArray<TextureStageDestination, DWORD, 2> TO_D3D {
    {TextureStageDestination::Current, D3DTA_CURRENT},
    {TextureStageDestination::Temporary, D3DTA_TEMP},
  };
  static_assert(TEXTURE_STAGE_DESTINATION_COUNT == TO_D3D.size());

  return TO_D3D[dest];
}

inline auto to_d3d(const TextureCoordinateTransformMode mode)
  -> D3DTEXTURETRANSFORMFLAGS {
  static constexpr EnumArray<TextureCoordinateTransformMode,
                             D3DTEXTURETRANSFORMFLAGS, 5>
    TO_D3D {
      {TextureCoordinateTransformMode::Disabled, D3DTTFF_DISABLE},
      {TextureCoordinateTransformMode::Count1, D3DTTFF_COUNT1},
      {TextureCoordinateTransformMode::Count2, D3DTTFF_COUNT2},
      {TextureCoordinateTransformMode::Count3, D3DTTFF_COUNT3},
      {TextureCoordinateTransformMode::Count4, D3DTTFF_COUNT4},
    };
  static_assert(TEXTURE_COORDINATE_TRANSFORM_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

inline auto to_d3d(const TransformState state) -> D3DTRANSFORMSTATETYPE {
  static constexpr EnumArray<TransformState, D3DTRANSFORMSTATETYPE, 11> TO_D3D {
    {TransformState::ViewToClip, D3DTS_PROJECTION},
    {TransformState::WorldToView, D3DTS_VIEW},
    {TransformState::LocalToWorld, D3DTS_WORLDMATRIX(0)},
    {TransformState::Texture0, D3DTS_TEXTURE0},
    {TransformState::Texture1, D3DTS_TEXTURE1},
    {TransformState::Texture2, D3DTS_TEXTURE2},
    {TransformState::Texture3, D3DTS_TEXTURE3},
    {TransformState::Texture4, D3DTS_TEXTURE4},
    {TransformState::Texture5, D3DTS_TEXTURE5},
    {TransformState::Texture6, D3DTS_TEXTURE6},
    {TransformState::Texture7, D3DTS_TEXTURE7},
  };
  static_assert(TRANSFORM_STATE_COUNT == TO_D3D.size());

  return TO_D3D[state];
}

inline auto to_d3d(const BorderColor borderColor, const Color& custom)
  -> D3DCOLOR {
  if (borderColor == BorderColor::Custom) {
    return to_d3d_color(custom);
  }

  static constexpr EnumArray<BorderColor, D3DCOLOR, 3> TO_D3D {
    {BorderColor::BlackTransparent, D3DCOLOR_ARGB(0, 0, 0, 0)},
    {BorderColor::BlackOpaque, D3DCOLOR_ARGB(255, 0, 0, 0)},
    {BorderColor::WhiteOpaque, D3DCOLOR_ARGB(255, 255, 255, 255)},
  };
  // don't count Custom
  static_assert(BORDER_COLOR_COUNT - 1 == TO_D3D.size());

  return TO_D3D[borderColor];
}

constexpr auto to_d3d_fvf(const VertexLayout layout) -> DWORD {
  // TODO: needs some form of validation

  DWORD fvf {0ul};

  // TODO: values >= 8 invalidate the fvf
  i32 numTexCoords {0};

  for (const VertexElement& element : layout) {
    switch (element) {
    case VertexElement::Position3F32:
      fvf |= D3DFVF_XYZ;
      break;

    case VertexElement::PositionTransformed4F32:
      fvf |= D3DFVF_XYZRHW;
      break;

    case VertexElement::Normal3F32:
      fvf |= D3DFVF_NORMAL;
      break;

    case VertexElement::PointSize1F32:
      fvf |= D3DFVF_PSIZE;
      break;

    case VertexElement::ColorDiffuse1U32A8R8G8B8:
      fvf |= D3DFVF_DIFFUSE;
      break;

    case VertexElement::ColorSpecular1U32A8R8G8B8:
      fvf |= D3DFVF_SPECULAR;
      break;

    case VertexElement::TextureCoords1F32:
      fvf |= D3DFVF_TEXCOORDSIZE1(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords2F32:
      fvf |= D3DFVF_TEXCOORDSIZE2(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords3F32:
      fvf |= D3DFVF_TEXCOORDSIZE3(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords4F32:
      fvf |= D3DFVF_TEXCOORDSIZE4(numTexCoords);
      ++numTexCoords;
      break;
    }
  }

  fvf |= std::min(8, numTexCoords) << D3DFVF_TEXCOUNT_SHIFT;

  return fvf;
}

inline auto to_d3d(const DirectionalLightData& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_DIRECTIONAL;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuse);
  d3dLight.Specular = to_d3d_color_value(light.specular);
  d3dLight.Ambient = to_d3d_color_value(light.ambient);
  d3dLight.Direction = to_d3d(light.directionInWorld);

  return d3dLight;
}

inline auto to_d3d(const PointLightData& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_POINT;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuse);
  d3dLight.Specular = to_d3d_color_value(light.specular);
  d3dLight.Ambient = to_d3d_color_value(light.ambient);
  d3dLight.Position = to_d3d(light.positionInWorld);
  d3dLight.Range = light.rangeInWorld;
  d3dLight.Attenuation0 = light.attenuation0;
  d3dLight.Attenuation1 = light.attenuation1;
  d3dLight.Attenuation2 = light.attenuation2;

  return d3dLight;
}

inline auto to_d3d(const SpotLightData& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_SPOT;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuse);
  d3dLight.Specular = to_d3d_color_value(light.specular);
  d3dLight.Ambient = to_d3d_color_value(light.ambient);
  d3dLight.Position = to_d3d(light.positionInWorld);
  d3dLight.Direction = to_d3d(light.directionInWorld);
  d3dLight.Range = light.rangeInWorld;
  d3dLight.Falloff = light.falloff;
  d3dLight.Attenuation0 = light.attenuation0;
  d3dLight.Attenuation1 = light.attenuation1;
  d3dLight.Attenuation2 = light.attenuation2;
  d3dLight.Theta = light.theta.radians();
  d3dLight.Phi = light.phi.radians();

  return d3dLight;
}

} // namespace basalt::gfx
