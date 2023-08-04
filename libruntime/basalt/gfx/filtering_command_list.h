#pragma once

#include <basalt/gfx/device_state_cache.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

namespace basalt::gfx {

// command list with redundant state filtering
class FilteringCommandList final {
public:
  auto clear_attachments(Attachments, const Color& = {}, f32 depth = 0.0f,
                         u32 stencil = 0) -> void;
  auto draw(u32 firstVertex, u32 vertexCount) -> void;
  auto draw_indexed(i32 vertexOffset, u32 minIndex, u32 numVertices,
                    u32 firstIndex, u32 indexCount) -> void;
  auto bind_pipeline(Pipeline) -> void;
  auto bind_vertex_buffer(VertexBuffer, uDeviceSize offsetInBytes = 0) -> void;
  auto bind_index_buffer(IndexBuffer) -> void;
  auto bind_sampler(Sampler) -> void;
  auto bind_texture(Texture) -> void;
  auto set_blend_constant(const Color&) -> void;
  auto set_transform(TransformState, const Matrix4x4f32&) -> void;
  auto set_ambient_light(const Color&) -> void;
  auto set_lights(gsl::span<const LightData>) -> void;
  auto set_material(const Color& diffuse, const Color& ambient = {},
                    const Color& emissive = {}, const Color& specular = {},
                    f32 specularPower = 0) -> void;
  auto set_fog_parameters(const Color&, f32 start, f32 end, f32 density = 0)
    -> void;
  auto set_reference_alpha(u8) -> void;

  auto cmd_list() -> CommandList&;

  [[nodiscard]] auto take_cmd_list() -> CommandList;

private:
  CommandList mCommandList;
  DeviceStateCache mDeviceState;
};

} // namespace basalt::gfx
