#pragma once
#ifndef BASALT_RUNTIME_GFX_TYPES_H
#define BASALT_RUNTIME_GFX_TYPES_H

#include "runtime/gfx/backend/render_command.h"
#include "runtime/gfx/backend/Types.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Types.h"

namespace basalt::gfx {

using backend::DirectionalLight;

struct RenderComponent final {
  backend::MeshHandle mMesh;
  backend::TextureHandle mTexture;
  Color mDiffuseColor;
  Color mAmbientColor;
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
