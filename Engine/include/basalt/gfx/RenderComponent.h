#pragma once
#ifndef BS_GFX_RENDER_COMPONENT_H
#define BS_GFX_RENDER_COMPONENT_H

#include <basalt/common/Color.h>

#include "backend/RenderCommand.h"

namespace basalt::gfx {

struct RenderComponent final {
  backend::MeshHandle mMesh;
  backend::TextureHandle mTexture;
  Color mDiffuseColor;
  Color mAmbientColor;
  i8 mRenderFlags = 0;
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

#endif // BS_GFX_RENDER_COMPONENT_H
