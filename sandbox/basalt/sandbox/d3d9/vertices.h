#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

namespace d3d9 {

struct Vertices final : basalt::View {
  explicit Vertices(basalt::Engine&);

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = default;

  ~Vertices() noexcept override;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline {basalt::gfx::Pipeline::null()};
  basalt::gfx::VertexBuffer mVertexBuffer {basalt::gfx::VertexBuffer::null()};

  auto on_draw(const DrawContext&) -> basalt::gfx::CommandList override;
};

} // namespace d3d9
