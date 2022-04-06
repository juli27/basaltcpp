#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/gfx/backend/types.h>

#include <array>

namespace basalt::gfx {

struct D3D9FixedVertexShader {
  static auto from(FixedVertexShaderCreateInfo const*) -> D3D9FixedVertexShader;

  D3DSHADEMODE shadeMode{D3DSHADE_GOURAUD};
  bool lightingEnabled{false};
  bool specularEnabled{false};
  bool vertexColorEnabled{true};
  bool normalizeViewSpaceNormals{false};
  D3DMATERIALCOLORSOURCE diffuseSource{D3DMCS_COLOR1};
  D3DMATERIALCOLORSOURCE ambientSource{D3DMCS_MATERIAL};
  D3DMATERIALCOLORSOURCE emissiveSource{D3DMCS_MATERIAL};
  D3DMATERIALCOLORSOURCE specularSource{D3DMCS_COLOR2};
};

struct D3D9Fog {
  static auto from(FixedVertexShaderCreateInfo const*,
                   FixedFragmentShaderCreateInfo const*) -> D3D9Fog;

  bool enabled{false};
  D3DFOGMODE vertexMode{D3DFOG_NONE};
  bool vertexRanged{false};
  D3DFOGMODE tableMode{D3DFOG_NONE};
};

struct D3D9TexStage {
  static auto from(FixedVertexShaderCreateInfo const*,
                   FixedFragmentShaderCreateInfo const*)
    -> std::array<D3D9TexStage, 8>;

  D3DTEXTUREOP colorOp{D3DTOP_DISABLE};
  DWORD colorArg1{D3DTA_TEXTURE};
  DWORD colorArg2{D3DTA_CURRENT};
  DWORD colorArg3{D3DTA_CURRENT};
  D3DTEXTUREOP alphaOp{D3DTOP_DISABLE};
  DWORD alphaArg1{D3DTA_TEXTURE};
  DWORD alphaArg2{D3DTA_CURRENT};
  DWORD alphaArg3{D3DTA_CURRENT};
  DWORD resultArg{D3DTA_CURRENT};
  DWORD coordinateIndex{0 | D3DTSS_TCI_PASSTHRU};
  D3DTEXTURETRANSFORMFLAGS coordinateTransformFlags{D3DTTFF_DISABLE};
  float bumpEnvMat00{1};
  float bumpEnvMat01{0};
  float bumpEnvMat10{0};
  float bumpEnvMat11{1};
  float bumpEnvLScale{1};
  float bumpEnvLOffset{0};
};

struct D3D9StencilOpState {
  static auto from(StencilOpState const&) noexcept -> D3D9StencilOpState;

  D3DCMPFUNC func{D3DCMP_ALWAYS};
  D3DSTENCILOP failOp{D3DSTENCILOP_KEEP};
  D3DSTENCILOP passDepthFailOp{D3DSTENCILOP_KEEP};
  D3DSTENCILOP passDepthPassOp{D3DSTENCILOP_KEEP};
};

struct D3D9Pipeline {
  static auto from(PipelineCreateInfo const& desc) -> D3D9Pipeline;

  DWORD fvf{};
  D3DPRIMITIVETYPE primitiveType{D3DPT_POINTLIST};
  D3D9FixedVertexShader vs{};
  D3D9Fog fog{};
  // TODO: custom allocate this in the pipeline memory pool
  // then use a span here
  std::array<D3D9TexStage, 8> textureStages{};
  D3DCULL cullMode{D3DCULL_NONE};
  D3DFILLMODE fillMode{D3DFILL_SOLID};
  D3DZBUFFERTYPE zEnabled{D3DZB_FALSE};
  D3DCMPFUNC zFunc{D3DCMP_ALWAYS};
  BOOL zWriteEnabled{FALSE};
  bool stencilEnabled{false};
  bool twoSidedStencilEnabled{false};
  D3D9StencilOpState cwStencilState{};
  D3D9StencilOpState ccwStencilState{};
  BOOL dithering{FALSE};
  bool alphaTestEnabled{false};
  D3DCMPFUNC alphaFunc{D3DCMP_ALWAYS};
  bool alphaBlendEnabled{false};
  D3DBLEND srcBlend{D3DBLEND_ONE};
  D3DBLEND destBlend{D3DBLEND_ZERO};
  D3DBLENDOP blendOp{D3DBLENDOP_ADD};
};

} // namespace basalt::gfx
