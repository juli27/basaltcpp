#pragma once

#include "types.h"
#include "vertex_layout.h"

#include <basalt/api/base/types.h>

#include <gsl/span>

namespace basalt::gfx {

struct StencilOpState {
  TestPassCond test{TestPassCond::Always};
  StencilOp failOp{StencilOp::Keep};
  StencilOp passDepthFailOp{StencilOp::Keep};
  StencilOp passDepthPassOp{StencilOp::Keep};
};

struct PipelineCreateInfo {
  // nullptr -> default fixed vertex shader
  FixedVertexShaderCreateInfo const* vertexShader{nullptr};
  // nullptr -> default fixed fragment shader
  FixedFragmentShaderCreateInfo const* fragmentShader{nullptr};
  VertexLayoutSpan vertexLayout;
  PrimitiveType primitiveType{PrimitiveType::PointList};
  CullMode cullMode{CullMode::None};
  FillMode fillMode{FillMode::Solid};
  TestPassCond depthTest{TestPassCond::Always};
  bool depthWriteEnable{false};
  StencilOpState frontFaceStencilOp;
  StencilOpState backFaceStencilOp;
  bool dithering{false};
  TestPassCond alphaTest{TestPassCond::Always};
  BlendFactor srcBlendFactor{BlendFactor::One};
  BlendFactor destBlendFactor{BlendFactor::Zero};
  BlendOp blendOp{BlendOp::Add};
};

struct FixedFragmentShaderCreateInfo {
  gsl::span<TextureStage const> textureStages;
  // this overrides vertex fog
  FogMode fog{FogMode::None};
};

struct FixedVertexShaderCreateInfo {
  gsl::span<TextureCoordinateSet const> textureCoordinateSets;
  ShadeMode shadeMode{ShadeMode::Gouraud};
  bool lightingEnabled{false};
  bool specularEnabled{false};
  bool vertexColorEnabled{true};
  bool normalizeViewSpaceNormals{false};
  MaterialColorSource diffuseSource{MaterialColorSource::DiffuseVertexColor};
  MaterialColorSource specularSource{MaterialColorSource::SpecularVertexColor};
  MaterialColorSource ambientSource{MaterialColorSource::Material};
  MaterialColorSource emissiveSource{MaterialColorSource::Material};
  FogMode fog{FogMode::None};
  bool fogRangeBased{false};
};

struct TextureCoordinateSet {
  u8 stageIndex{0};
  TextureCoordinateSrc src{TextureCoordinateSrc::Vertex};
  u8 srcIndex{0};
  TextureCoordinateTransformMode transformMode{
    TextureCoordinateTransformMode::Disabled};
  bool projected{false};
};

struct TextureStageArgument {
  TextureStageSrc src{TextureStageSrc::Current};
  TextureStageSrcMod modifier{TextureStageSrcMod::None};
};

struct TextureStage {
  TextureOp colorOp{TextureOp::Modulate};
  TextureStageArgument colorArg1{TextureStageSrc::SampledTexture};
  TextureStageArgument colorArg2;
  TextureStageArgument colorArg3;
  TextureOp alphaOp{TextureOp::Replace};
  TextureStageArgument alphaArg1{TextureStageSrc::SampledTexture};
  TextureStageArgument alphaArg2;
  TextureStageArgument alphaArg3;
  TextureStageDestination dest{TextureStageDestination::Current};
  // TODO: replace with Matrix2x2
  f32 bumpEnvMat00{1.0f};
  f32 bumpEnvMat01{0.0f};
  f32 bumpEnvMat10{0.0f};
  f32 bumpEnvMat11{1.0f};
  f32 bumpEnvLuminanceScale{1.0f};
  f32 bumpEnvLuminanceOffset{0.0f};
};

} // namespace basalt::gfx
