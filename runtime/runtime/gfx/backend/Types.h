#pragma once
#ifndef BASALT_GFX_BACKEND_TYPES_H
#define BASALT_GFX_BACKEND_TYPES_H

#include "runtime/shared/Handle.h"
#include "runtime/shared/Types.h"

#include <vector>

namespace basalt::gfx::backend {

enum class VertexElement : u8 {
  Position3F32,
  PositionTransformed4F32,
  Normal3F32,
  ColorDiffuse1U32,
  ColorSpecular1U32,
  TextureCoords2F32
};


using VertexLayout = std::vector<VertexElement>;


enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan
};


namespace detail {

struct MeshTypeTag {
};

struct TextureTypeTag {
};

struct ModelTypeTag {
};

} // namespace detail

using MeshHandle = Handle<detail::MeshTypeTag>;
using TextureHandle = Handle<detail::TextureTypeTag>;
using ModelHandle = Handle<detail::ModelTypeTag>;

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_TYPES_H
