#pragma once

#include <runtime/gfx/camera.h>

#include <runtime/gfx/backend/device.h>
#include <runtime/gfx/backend/types.h>

#include <runtime/shared/types.h>

#include <array>

namespace d3d9 {

auto create_default_camera() -> basalt::gfx::Camera;

template <typename T, std::size_t Size>
auto add_triangle_list_mesh(basalt::gfx::Device& device,
                            std::array<T, Size>& data,
                            const basalt::gfx::VertexLayout& layout)
  -> basalt::gfx::MeshHandle {
  return device.add_mesh(data.data(), static_cast<basalt::i32>(data.size()),
                         layout, basalt::gfx::PrimitiveType::TriangleList);
}

template <typename T, std::size_t Size>
auto add_triangle_strip_mesh(basalt::gfx::Device& device,
                             std::array<T, Size>& data,
                             const basalt::gfx::VertexLayout& layout)
  -> basalt::gfx::MeshHandle {
  return device.add_mesh(data.data(), static_cast<basalt::i32>(data.size()),
                         layout, basalt::gfx::PrimitiveType::TriangleStrip);
}

} // namespace d3d9
