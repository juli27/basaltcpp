#include <basalt/sandbox/tribase/02-03_first_triangle.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <utility>

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::FillMode;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

using gsl::span;

using std::array;
using std::byte;

namespace tribase {

namespace {

struct Vertex final {
  Vector3f32 pos{};
  ColorEncoding::A8R8G8B8 diffuse{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
  };
};

constexpr auto TRIANGLE_VERTICES = array{
  Vertex{{0.0f, 1.0f, 0.0f}, Colors::RED.to_argb()},
  Vertex{{1.0f, -1.0f, 0.0f}, Colors::GREEN.to_argb()},
  Vertex{{-1.0f, -1.0f, 0.0f}, Colors::BLUE.to_argb()},
};

constexpr auto QUAD_VERTICES = array{
  Vertex{{1.0f, -1.0f, 0.0f}, 0xff00ff00_a8r8g8b8},
  Vertex{{-1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
  Vertex{{1.0f, 1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
  Vertex{{-1.0f, 1.0f, 0.0f}, 0xffff00ff_a8r8g8b8},
};

} // namespace

FirstTriangle::FirstTriangle(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mVertexBuffer{[&] {
    auto const vertexData = as_bytes(span{TRIANGLE_VERTICES});

    // need space for a quad
    constexpr auto vbSize = 4 * sizeof(Vertex);

    return mGfxCache->create_vertex_buffer({vbSize, Vertex::sLayout},
                                           vertexData);
  }()}
  , mPipeline{[&] {
    auto desc = PipelineDescriptor{};
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    desc.dithering = true;
    return mGfxCache->create_pipeline(desc);
  }()}
  , mQuadPipeline{[&] {
    auto desc = PipelineDescriptor{};
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleStrip;
    desc.dithering = true;
    return mGfxCache->create_pipeline(desc);
  }()}
  , mWireframePipeline{[&] {
    auto desc = PipelineDescriptor{};
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    desc.fillMode = FillMode::Wireframe;
    desc.dithering = true;
    return mGfxCache->create_pipeline(desc);
  }()} {
}

auto FirstTriangle::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;

  if (ImGui::Begin("Settings##TribaseDreieck")) {
    auto const uploadDefaultTriangle = [&] {
      mGfxCache->with_mapping_of(mVertexBuffer, [](span<byte> const data) {
        auto const vertexData = as_bytes(span{TRIANGLE_VERTICES});
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });
    };

    if (ImGui::RadioButton("No Exercise", &mCurrentExercise, 0)) {
      mScale = 1.0f;

      uploadDefaultTriangle();
    }

    if (ImGui::RadioButton("Exercise 1", &mCurrentExercise, 1)) {
      mScale = 1.0f;
      mTime = {};

      uploadDefaultTriangle();
    }

    if (ImGui::RadioButton("Exercise 2", &mCurrentExercise, 2)) {
      mScale = 1.0f;

      mGfxCache->with_mapping_of(mVertexBuffer, [](span<byte> const data) {
        auto const vertexData = as_bytes(span{QUAD_VERTICES});
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });
    }

    if (ImGui::RadioButton("Exercise 3", &mCurrentExercise, 3)) {
      mScale = 1.0f;

      uploadDefaultTriangle();
    }

    if (ImGui::RadioButton("Exercise 4", &mCurrentExercise, 4)) {
      mTime = {};

      uploadDefaultTriangle();
    }
  }

  ImGui::End();

  auto const t = mTime.count();
  auto const dt = ctx.deltaTime.count();

  // 90 deg per second
  mRotationY += Angle::degrees(90.0f * dt);

  if (mCurrentExercise == 1) {
    auto const alpha = 2.0f * t;
    auto const value1 = std::sin(alpha) / 2.0f + 0.5f;
    auto const value2 = std::cos(alpha) / 2.0f + 0.5f;

    auto const triangleVertices = array{
      Vertex{{0.0f, 1.0f, 0.0f},
             Color::from_non_linear(value1, value2, 0.0f).to_argb()},
      Vertex{{1.0f, -1.0f, 0.0f},
             Color::from_non_linear(0.0f, value1, value2).to_argb()},
      Vertex{{-1.0f, -1.0f, 0.0f},
             Color::from_non_linear(value2, 0.0f, value1).to_argb()},
    };

    mGfxCache->with_mapping_of(mVertexBuffer, [&](span<byte> const data) {
      auto const vertexData = as_bytes(span{triangleVertices});
      std::copy_n(vertexData.begin(),
                  std::min(vertexData.size_bytes(), data.size_bytes()),
                  data.begin());
    });
  }

  if (mCurrentExercise == 4) {
    mScale = 1.0f + std::sin(PI / 2.0f * t) / 2.0f;
  }

  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f);

  cmdList.bind_pipeline([&] {
    if (mCurrentExercise == 2) {
      return mQuadPipeline;
    }
    if (mCurrentExercise == 3) {
      return mWireframePipeline;
    }

    return mPipeline;
  }());

  auto const& drawCtx = ctx.drawCtx;

  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f));

  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::scaling(mScale) *
                          Matrix4x4f32::rotation_y(mRotationY) *
                          Matrix4x4f32::translation(0.0f, 0.0f, 2.0f));

  cmdList.bind_vertex_buffer(mVertexBuffer);

  auto const vertexCount = u32{mCurrentExercise == 2 ? 4u : 3u};
  cmdList.draw(0, vertexCount);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
