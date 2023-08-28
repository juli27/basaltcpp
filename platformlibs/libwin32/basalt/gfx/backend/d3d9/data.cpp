#include <basalt/gfx/backend/d3d9/data.h>

#include <basalt/gfx/backend/d3d9/conversions.h>

#include <array>

using std::array;

namespace basalt::gfx {

namespace {

auto is_no_op(const StencilOpState& op) -> bool {
  // fail op doesn't matter since the test has to always pass
  return op.test == TestPassCond::Always &&
         op.passDepthFailOp == StencilOp::Keep &&
         op.passDepthPassOp == StencilOp::Keep;
}

} // namespace

auto D3D9FixedVertexShader::from(const FixedVertexShaderCreateInfo* info)
  -> D3D9FixedVertexShader {
  if (!info) {
    return D3D9FixedVertexShader {};
  }

  return D3D9FixedVertexShader {
    to_d3d(info->shadeMode),         info->lightingEnabled,
    info->specularEnabled,           info->vertexColorEnabled,
    info->normalizeViewSpaceNormals, to_d3d(info->diffuseSource),
    to_d3d(info->ambientSource),     to_d3d(info->emissiveSource),
    to_d3d(info->specularSource)};
}

auto D3D9Fog::from(const FixedVertexShaderCreateInfo* vs,
                   const FixedFragmentShaderCreateInfo* fs) -> D3D9Fog {
  D3DFOGMODE vertexMode {D3DFOG_NONE};
  bool vertexRanged {false};
  D3DFOGMODE tableMode {D3DFOG_NONE};
  if (vs) {
    vertexMode = to_d3d(vs->fog);
    vertexRanged = vs->fogRangeBased;
  }
  if (fs) {
    tableMode = to_d3d(fs->fog);
  }

  const bool enabled {vertexMode != D3DFOG_NONE || tableMode != D3DFOG_NONE};

  return D3D9Fog {enabled, vertexMode, vertexRanged, tableMode};
}

auto D3D9TexStage::from(const FixedVertexShaderCreateInfo* vs,
                        const FixedFragmentShaderCreateInfo* fs)
  -> std::array<D3D9TexStage, 8> {
  array<D3D9TexStage, 8> stages {};
  if (!fs) {
    return stages;
  }

  u8 i {0};
  for (const auto& stage : fs->textureStages) {
    auto& d3d9Stage {stages[i]};
    d3d9Stage.colorOp = to_d3d(stage.colorOp);
    d3d9Stage.colorArg1 = to_d3d(stage.colorArg1);
    d3d9Stage.colorArg2 = to_d3d(stage.colorArg2);
    d3d9Stage.colorArg3 = to_d3d(stage.colorArg3);
    d3d9Stage.alphaOp = to_d3d(stage.alphaOp);
    d3d9Stage.alphaArg1 = to_d3d(stage.alphaArg1);
    d3d9Stage.alphaArg2 = to_d3d(stage.alphaArg2);
    d3d9Stage.alphaArg3 = to_d3d(stage.alphaArg3);
    d3d9Stage.bumpEnvMat00 = stage.bumpEnvMat00;
    d3d9Stage.bumpEnvMat01 = stage.bumpEnvMat01;
    d3d9Stage.bumpEnvMat10 = stage.bumpEnvMat10;
    d3d9Stage.bumpEnvMat11 = stage.bumpEnvMat11;
    d3d9Stage.bumpEnvLScale = stage.bumpEnvLuminanceScale;
    d3d9Stage.bumpEnvLOffset = stage.bumpEnvLuminanceOffset;

    i++;
  }

  if (vs) {
    for (const auto& coordinateSet : vs->textureCoordinateSets) {
      auto& d3d9Stage {stages[coordinateSet.stageIndex]};

      d3d9Stage.coordinateIndex =
        coordinateSet.srcIndex | to_d3d(coordinateSet.src);
      d3d9Stage.coordinateTransformFlags = to_d3d(coordinateSet.transformMode);

      if (coordinateSet.projected) {
        d3d9Stage.coordinateTransformFlags =
          static_cast<D3DTEXTURETRANSFORMFLAGS>(
            d3d9Stage.coordinateTransformFlags | D3DTTFF_PROJECTED);
      }
    }
  }

  return stages;
}

auto D3D9StencilOpState::from(const StencilOpState& stencilOp) noexcept
  -> D3D9StencilOpState {
  return D3D9StencilOpState {to_d3d(stencilOp.test), to_d3d(stencilOp.failOp),
                             to_d3d(stencilOp.passDepthFailOp),
                             to_d3d(stencilOp.passDepthPassOp)};
}

auto D3D9Pipeline::from(const PipelineDescriptor& desc) -> D3D9Pipeline {
  // TODO: is there a benefit to turn off z testing when func = Always
  // and with writing disabled?
  const D3DZBUFFERTYPE zEnabled {desc.depthTest == TestPassCond::Always &&
                                     !desc.depthWriteEnable
                                   ? D3DZB_FALSE
                                   : D3DZB_TRUE};

  const bool stencilEnabled {!is_no_op(desc.frontFaceStencilOp) ||
                             !is_no_op(desc.backFaceStencilOp)};
  const bool twoSidedStencilEnabled {desc.cullMode == CullMode::None &&
                                     stencilEnabled &&
                                     !is_no_op(desc.backFaceStencilOp)};

  const bool alphaTestEnabled {desc.alphaTest != TestPassCond::Always};
  const bool alphaBlendEnabled {desc.blendOp != BlendOp::Add ||
                                desc.srcBlendFactor != BlendFactor::One ||
                                desc.destBlendFactor != BlendFactor::Zero};

  return D3D9Pipeline {
    to_d3d_fvf(desc.vertexLayout),
    to_d3d(desc.primitiveType),
    D3D9FixedVertexShader::from(desc.vertexShader),
    D3D9Fog::from(desc.vertexShader, desc.fragmentShader),
    D3D9TexStage::from(desc.vertexShader, desc.fragmentShader),
    to_d3d(desc.cullMode),
    to_d3d(desc.fillMode),
    zEnabled,
    to_d3d(desc.depthTest),
    desc.depthWriteEnable,
    stencilEnabled,
    twoSidedStencilEnabled,
    D3D9StencilOpState::from(desc.frontFaceStencilOp),
    D3D9StencilOpState::from(desc.backFaceStencilOp),
    desc.dithering,
    alphaTestEnabled,
    to_d3d(desc.alphaTest),
    alphaBlendEnabled,
    to_d3d(desc.srcBlendFactor),
    to_d3d(desc.destBlendFactor),
    to_d3d(desc.blendOp),
  };
}

} // namespace basalt::gfx
