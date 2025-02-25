#include "samples.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/shared/data.h>

#include <basalt/api/math/vector4.h>

#include <gsl/span>

#include <array>
#include <memory>

using std::array;

using gsl::span;

using namespace basalt::literals;
using basalt::Engine;
using basalt::Vector4f32;
using basalt::View;
using basalt::ViewPtr;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PipelineHandle;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCachePtr;
using basalt::gfx::VertexBufferHandle;
using basalt::gfx::VertexElement;

namespace {

class SimpleViewRendering final : public View {
  struct Vertex {
    Vector4f32 pos;
    ColorEncoding::A8R8G8B8 color;

    static constexpr auto sLayout = basalt::gfx::make_vertex_layout<
      VertexElement::PositionTransformed4F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8>();
  };

public:
  explicit SimpleViewRendering(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mVertexBuffer{[&] {
      constexpr auto vertices = array{
        Vertex{{150.0f, 50.0f, 0.5f, 1.0f}, 0xffff0000_a8r8g8b8},
        Vertex{{250.0f, 250.0f, 0.5f, 1.0f}, 0xff00ff00_a8r8g8b8},
        Vertex{{50.0f, 250.0f, 0.5f, 1.0f}, 0xff00ffff_a8r8g8b8},
      };

      auto const vertexData = as_bytes(span{vertices});

      return mGfxCache->create_vertex_buffer(
        {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
    }()}
    , mPipeline{[&] {
      auto desc = PipelineCreateInfo{};
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = PrimitiveType::TriangleList;

      return mGfxCache->create_pipeline(desc);
    }()} {
  }

protected:
  auto on_update(UpdateContext& ctx) -> void override {
    auto constexpr background = Color::from_non_linear_rgba8(32, 32, 32);

    auto cmdList = CommandList{};
    cmdList.clear_attachments(Attachments{Attachment::RenderTarget},
                              background);
    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, 3);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }

private:
  ResourceCachePtr mGfxCache;
  VertexBufferHandle mVertexBuffer;
  PipelineHandle mPipeline;
};

} // namespace

auto Samples::new_simple_view_rendering_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<SimpleViewRendering>(engine);
}
