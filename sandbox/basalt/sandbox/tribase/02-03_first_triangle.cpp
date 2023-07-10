#include <basalt/sandbox/tribase/02-03_first_triangle.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>

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
  array<f32, 3> pos {};
  ColorEncoding::A8R8G8B8 diffuse {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

constexpr array<Vertex, 3> TRIANGLE_VERTICES {
  Vertex {{0.0f, 1.0f, 0.0f}, Colors::RED.to_argb()},
  Vertex {{1.0f, -1.0f, 0.0f}, Colors::GREEN.to_argb()},
  Vertex {{-1.0f, -1.0f, 0.0f}, Colors::BLUE.to_argb()},
};

constexpr array<Vertex, 4> QUAD_VERTICES {
  Vertex {{1.0f, -1.0f, 0.0f}, 0xff00ff00_a8r8g8b8},
  Vertex {{-1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
  Vertex {{1.0f, 1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
  Vertex {{-1.0f, 1.0f, 0.0f}, 0xffff00ff_a8r8g8b8},
};

} // namespace

FirstTriangle::FirstTriangle(Engine& engine)
  : mGfxCache {engine.gfx_resource_cache()} {
  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.dithering = true;
  mPipeline = mGfxCache.create_pipeline(pipelineDesc);

  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  mQuadPipeline = mGfxCache.create_pipeline(pipelineDesc);

  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.fillMode = FillMode::Wireframe;
  mWireframePipeline = mGfxCache.create_pipeline(pipelineDesc);

  const span vertexData {as_bytes(span {TRIANGLE_VERTICES})};
  mVertexBuffer = mGfxCache.create_vertex_buffer(
    {4 * sizeof(Vertex), Vertex::sLayout}, vertexData);
}

FirstTriangle::~FirstTriangle() noexcept {
  mGfxCache.destroy(mVertexBuffer);
  mGfxCache.destroy(mWireframePipeline);
  mGfxCache.destroy(mQuadPipeline);
  mGfxCache.destroy(mPipeline);
}

auto FirstTriangle::on_update(UpdateContext& ctx) -> void {
  const f32 dt {ctx.deltaTime.count()};

  // 90 deg per second
  mRotationY += Angle::degrees(90.0f * dt);
  while (mRotationY.radians() > PI) {
    mRotationY -= Angle::radians(PI * 2.0f);
  }

  if (ImGui::Begin("Settings##TribaseDreieck")) {
    const auto uploadDefaultTriangle {[this] {
      mGfxCache.with_mapping_of(mVertexBuffer, [](const span<byte> data) {
        const span vertexData {as_bytes(span {TRIANGLE_VERTICES})};
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });
    }};

    if (ImGui::RadioButton("No Exercise", &mCurrentExercise, 0)) {
      mScale = 1.0f;

      uploadDefaultTriangle();
    }

    if (ImGui::RadioButton("Exercise 1", &mCurrentExercise, 1)) {
      mScale = 1.0f;
      mTimeAccum = 0.0;

      uploadDefaultTriangle();
    }

    if (ImGui::RadioButton("Exercise 2", &mCurrentExercise, 2)) {
      mScale = 1.0f;

      mGfxCache.with_mapping_of(mVertexBuffer, [](const span<byte> data) {
        const span vertexData {as_bytes(span {QUAD_VERTICES})};
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
      mTimeAccum = 0.0;

      uploadDefaultTriangle();
    }

    if (mCurrentExercise == 1) {
      mTimeAccum += dt;

      const f32 time {static_cast<f32>(mTimeAccum * 2.0)};

      const f32 value1 {std::sin(time) / 2.0f + 0.5f};
      const f32 value2 {std::cos(time) / 2.0f + 0.5f};

      const array<Vertex, 3> triangleVertices {
        Vertex {0.0f, 1.0f, 0.0f,
                Color::from_non_linear(value1, value2, 0.0f).to_argb()},
        Vertex {1.0f, -1.0f, 0.0f,
                Color::from_non_linear(0.0f, value1, value2).to_argb()},
        Vertex {-1.0f, -1.0f, 0.0f,
                Color::from_non_linear(value2, 0.0f, value1).to_argb()},
      };

      mGfxCache.with_mapping_of(mVertexBuffer, [&](const span<byte> data) {
        const span vertexData {as_bytes(span {triangleVertices})};
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });
    }

    if (mCurrentExercise == 4) {
      mTimeAccum += dt;

      mScale = 1.0f + std::sin(PI / 2.0f * static_cast<f32>(mTimeAccum)) / 2.0f;
    }
  }

  ImGui::End();

  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f);

  const Pipeline pipeline {[this] {
    if (mCurrentExercise == 2) {
      return mQuadPipeline;
    }
    if (mCurrentExercise == 3) {
      return mWireframePipeline;
    }

    return mPipeline;
  }()};

  cmdList.bind_pipeline(pipeline);
  cmdList.bind_vertex_buffer(mVertexBuffer);

  const DrawContext& drawCtx {ctx.drawCtx};
  const f32 aspectRatio {drawCtx.viewport.aspect_ratio()};

  const auto viewToClip {
    Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f)};
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  const auto localToWorld {Matrix4x4f32::scaling(mScale) *
                           Matrix4x4f32::rotation_y(mRotationY) *
                           Matrix4x4f32::translation(0.0f, 0.0f, 2.0f)};
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld);

  const u32 vertexCount {
    [this]() -> u32 { return mCurrentExercise == 2 ? 4 : 3; }()};
  cmdList.draw(0, vertexCount);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
