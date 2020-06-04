#pragma once
#ifndef BASALT_RUNTIME_GFX_TYPES_H
#define BASALT_RUNTIME_GFX_TYPES_H

#include "runtime/gfx/backend/render_command.h"
#include "runtime/gfx/backend/Types.h"

#include "runtime/math/Mat4.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Types.h"

#include <memory>

namespace basalt::gfx {

struct RenderComponent final {
  backend::MeshHandle mMesh;
  backend::TextureHandle mTexture;
  backend::ModelHandle model;
  Color mDiffuseColor;
  Color mAmbientColor;
  math::Mat4f32 texTransform {math::Mat4f32::identity()};
  backend::TexCoordinateSrc tcs {backend::TexCoordinateSrc::Vertex};
  u8 mRenderFlags = backend::RenderFlagNone;
};

// GfxComponent
// Geometry
// Materials
//
// RenderComponent
// list of parts/nodes
//   geometry
//   material

} // namespace basalt::gfx

#endif // BASALT_RUNTIME_GFX_TYPES_H
