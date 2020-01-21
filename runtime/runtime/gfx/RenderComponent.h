#pragma once
#ifndef BASALT_GFX_RENDER_COMPONENT_H
#define BASALT_GFX_RENDER_COMPONENT_H

#include "backend/RenderCommand.h"

#include "runtime/shared/Color.h"

namespace basalt::gfx {

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

#endif // BASALT_GFX_RENDER_COMPONENT_H
