#pragma once

#include <basalt/api/gfx/device_state_cache.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>
#include <basalt/api/math/types.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

namespace basalt::gfx {

// command list with redundant state filtering
struct FilteringCommandList final {
  void clear_attachments(Attachments, const Color&, f32 z, u32 stencil);
  void draw(u32 firstVertex, u32 vertexCount);
  void bind_pipeline(Pipeline);
  void bind_vertex_buffer(VertexBuffer, u64 offset);
  void bind_sampler(Sampler);
  void bind_texture(Texture);
  void set_transform(TransformState, const Matrix4x4f32&);
  void set_ambient_light(const Color&);
  void set_lights(gsl::span<const Light>);
  void set_material(const Color& diffuse, const Color& ambient,
                    const Color& emissive);

  void ext_draw_x_mesh(ext::XMesh, u32 subsetIndex);
  void ext_render_dear_imgui();

  [[nodiscard]] auto take_cmd_list() -> CommandList;

private:
  CommandList mCommandList;
  DeviceStateCache mDeviceState;
};

} // namespace basalt::gfx
