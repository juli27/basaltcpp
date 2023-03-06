#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <memory>
#include <memory_resource>
#include <type_traits>
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

  auto clear_attachments(Attachments, const Color&, f32 depth, u32 stencil)
    -> void;
  auto draw(u32 firstVertex, u32 vertexCount) -> void;
  auto draw_indexed(i32 vertexOffset, u32 minIndex, u32 numVertices,
                    u32 firstIndex, u32 indexCount) -> void;
  auto bind_pipeline(Pipeline) -> void;
  auto bind_vertex_buffer(VertexBuffer, uDeviceSize offset) -> void;
  auto bind_index_buffer(IndexBuffer) -> void;
  auto bind_sampler(Sampler) -> void;
  auto bind_texture(Texture) -> void;
  auto set_transform(TransformState, const Matrix4x4f32&) -> void;
  auto set_ambient_light(const Color&) -> void;
  auto set_lights(gsl::span<const Light>) -> void;
  auto set_material(const Color& diffuse, const Color& ambient,
                    const Color& emissive) -> void;

  auto ext_draw_x_mesh(ext::XMesh, u32 subsetIndex) -> void;
  auto ext_render_dear_imgui() -> void;

private:
  using CommandBuffer = std::pmr::monotonic_buffer_resource;

  std::unique_ptr<CommandBuffer> mBuffer;
  ListType mCommands;

  template <typename T>
  [[nodiscard]] auto allocate(const uSize count = 1) const -> gsl::span<T> {
    static_assert(std::is_trivially_destructible_v<T>);

    // TODO: overflow protection
    return {static_cast<T*>(mBuffer->allocate(sizeof(T) * count, alignof(T))),
            count};
  }

  template <typename T, typename... Args>
  auto add(Args&&...) -> void;
};

} // namespace basalt::gfx
