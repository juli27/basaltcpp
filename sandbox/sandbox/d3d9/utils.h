#pragma once
#ifndef SANDBOX_D3D9_UTILS_H
#define SANDBOX_D3D9_UTILS_H

#include <runtime/gfx/Camera.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/shared/Types.h>

#include <array>

namespace d3d9 {

auto create_default_camera() -> basalt::gfx::Camera;

template <typename T, std::size_t Size>
auto add_triangle_list_mesh(
  basalt::gfx::backend::IRenderer* renderer, std::array<T, Size>& data
, const basalt::gfx::backend::VertexLayout& layout
) -> basalt::gfx::backend::MeshHandle {
  return renderer->add_mesh(
    data.data(), static_cast<basalt::i32>(data.size()), layout
  , basalt::gfx::backend::PrimitiveType::TriangleList);
}

template <typename T, std::size_t Size>
auto add_triangle_strip_mesh(
  basalt::gfx::backend::IRenderer* renderer, std::array<T, Size>& data
, const basalt::gfx::backend::VertexLayout& layout
) -> basalt::gfx::backend::MeshHandle {
  return renderer->add_mesh(
    data.data(), static_cast<basalt::i32>(data.size()), layout
  , basalt::gfx::backend::PrimitiveType::TriangleStrip);
}

} // namespace d3d9

#endif // SANDBOX_D3D9_UTILS_H
