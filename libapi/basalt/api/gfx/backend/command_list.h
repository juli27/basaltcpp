#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/types.h>
#include <basalt/api/math/types.h>
#include <basalt/api/base/types.h>

#include <memory>
#include <memory_resource>
#include <type_traits>
#include <vector>

namespace basalt::gfx {

// serialized commands which the gfx device should execute
struct CommandList final {
private:
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

  void clear_attachments(Attachments, const Color&, f32 z, u32 stencil);
  void draw(u32 firstVertex, u32 vertexCount);
  void set_render_state(const RenderState&);
  void bind_pipeline(Pipeline);
  void bind_vertex_buffer(VertexBuffer, u64 offset);
  void bind_sampler(Sampler);
  void bind_texture(Texture);
  void set_transform(TransformState, const Mat4f32&);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_material(const Color& diffuse, const Color& ambient,
                    const Color& emissive);

  // TODO: stage currently not supported
  void set_texture_stage_state(u8 stage, TextureStageState, u32 value);

  void ext_draw_x_model(ext::XModel);
  void ext_render_dear_imgui();

private:
  using CommandBuffer = std::pmr::monotonic_buffer_resource;

  std::unique_ptr<CommandBuffer> mBuffer;
  ListType mCommands;

  template <typename T, typename... Args>
  void add(Args&&... args) {
    static_assert(std::is_base_of_v<Command, T>,
                  "CommandList only accepts commands derived from Command");
    static_assert(std::is_trivially_destructible_v<T>);

    mCommands.emplace_back(::new (mBuffer->allocate(sizeof(T), alignof(T)))
                             T(std::forward<Args>(args)...));
  }
};

} // namespace basalt::gfx
