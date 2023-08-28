#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <memory>
#include <memory_resource>
#include <vector>

namespace basalt::gfx {

// serialized commands which the gfx device should execute
class CommandList final {
  using ListType = std::vector<const Command*>;

public:
  using const_iterator = ListType::const_iterator;

  CommandList();

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) noexcept = default;

  ~CommandList() noexcept = default;

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList&&) noexcept -> CommandList& = default;

  [[nodiscard]] auto size() const noexcept -> uSize;

  [[nodiscard]] auto begin() const -> const_iterator;
  [[nodiscard]] auto end() const -> const_iterator;

  auto clear_attachments(Attachments, const Color& = {}, f32 depth = 0,
                         u32 stencil = 0) -> void;
  auto draw(u32 firstVertex, u32 vertexCount) -> void;
  auto draw_indexed(i32 vertexOffset, u32 minIndex, u32 numVertices,
                    u32 firstIndex, u32 indexCount) -> void;
  auto bind_pipeline(Pipeline) -> void;
  auto bind_vertex_buffer(VertexBuffer, uDeviceSize offsetInBytes = 0) -> void;
  auto bind_index_buffer(IndexBuffer) -> void;
  auto bind_sampler(u8 slot, Sampler) -> void;
  auto bind_texture(u8 slot, Texture) -> void;
  auto set_stencil_reference(u32) -> void;
  auto set_stencil_read_mask(u32) -> void;
  auto set_stencil_write_mask(u32) -> void;
  auto set_blend_constant(const Color&) -> void;
  auto set_transform(TransformState, const Matrix4x4f32&) -> void;
  auto set_ambient_light(const Color&) -> void;
  auto set_lights(gsl::span<const LightData>) -> void;
  auto set_material(const Color& diffuse, const Color& ambient = {},
                    const Color& emissive = {}, const Color& specular = {},
                    f32 specularPower = 0) -> void;
  auto set_fog_parameters(const Color& color, f32 start, f32 end,
                          f32 density = 0) -> void;
  auto set_reference_alpha(u8) -> void;
  auto set_texture_factor(const Color&) -> void;
  auto set_texture_stage_constant(u8 stageId, const Color&) -> void;

  // the following function templates are engine private (implementation is in
  // libRuntime: basalt/gfx/backend/command_list.h)
  template <typename T>
  [[nodiscard]] auto allocate(uSize count = 1) const -> gsl::span<T>;

  template <typename T, typename... Args>
  auto add(Args&&...) -> void;

private:
  using CommandBuffer = std::pmr::monotonic_buffer_resource;

  std::unique_ptr<CommandBuffer> mBuffer;
  ListType mCommands;
};

} // namespace basalt::gfx
