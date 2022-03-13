#include <basalt/sandbox/d3d9/matrices.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/mat4.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>

using std::array;
using std::byte;

using gsl::span;

using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::RectangleU16;
using basalt::Size2Du16;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::Camera;
using basalt::gfx::CommandList;
using basalt::gfx::Drawable;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCache;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBuffer;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

struct Matrices::MyDrawable final : Drawable {
  explicit MyDrawable(Engine& engine) noexcept
    : mResourceCache {engine.gfx_resource_cache()} {
    PipelineDescriptor pipelineDesc {};
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    mPipeline = mResourceCache.create_pipeline(pipelineDesc);

    struct Vertex final {
      f32 x;
      f32 y;
      f32 z;
      ColorEncoding::A8R8G8B8 color;
    };

    const array vertices {
      Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
      Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
      Vertex {0.0f, 1.0f, 0.0f,
              ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255)},
    };

    const auto vertexData {as_bytes(span {vertices})};

    const VertexBufferDescriptor vertexBufferDesc {
      vertexData.size_bytes(),
      VertexLayout {
        VertexElement::Position3F32,
        VertexElement::ColorDiffuse1U32A8R8G8B8,
      },
    };
    mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  MyDrawable(const MyDrawable&) = delete;
  MyDrawable(MyDrawable&&) noexcept = default;

  ~MyDrawable() noexcept override {
    mResourceCache.destroy(mVertexBuffer);
    mResourceCache.destroy(mPipeline);
  }

  auto operator=(const MyDrawable&) -> MyDrawable& = delete;
  auto operator=(MyDrawable&&) -> MyDrawable& = delete;

  void tick(const Engine& engine) {
    // 1 full rotation per second
    constexpr f32 twoPi {PI * 2.0f};
    mAngleYRad += twoPi * static_cast<f32>(engine.delta_time());
    if (mAngleYRad > PI) {
      mAngleYRad -= twoPi;
    }
  }

  auto draw(ResourceCache&, const Size2Du16 viewport, const RectangleU16&)
    -> std::tuple<CommandList, RectangleU16> override {
    CommandList cmdList {};

    cmdList.clear_attachments(Attachments {Attachment::Color}, Colors::BLACK,
                              1.0f, 0);

    cmdList.bind_pipeline(mPipeline);

    cmdList.set_transform(TransformState::ModelToWorld,
                          Mat4f32::rotation_y(mAngleYRad));
    cmdList.set_transform(TransformState::WorldToView, mCamera.view_matrix());
    cmdList.set_transform(TransformState::ViewToViewport,
                          mCamera.projection_matrix(viewport));

    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

    cmdList.draw(0, 3);

    return {std::move(cmdList), viewport.to_rectangle()};
  }

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};
  Camera mCamera {create_default_camera()};
  f32 mAngleYRad {};
};

Matrices::Matrices(Engine& engine)
  : mDrawable {std::make_shared<MyDrawable>(engine)} {
}

auto Matrices::drawable() -> basalt::gfx::DrawablePtr {
  return mDrawable;
}

void Matrices::on_tick(Engine& engine) {
  mDrawable->tick(engine);
}

} // namespace d3d9
