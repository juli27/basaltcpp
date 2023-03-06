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
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;

using gsl::span;

using std::array;
using std::byte;

namespace tribase {

namespace {

struct Vertex final {
  f32 x;
  f32 y;
  f32 z;
  ColorEncoding::A8R8G8B8 diffuse;
};

constexpr array<Vertex, 3> TRIANGLE_VERTICES {
  Vertex {0.0f, 1.0f, 0.0f, Colors::RED.to_argb()},
  Vertex {1.0f, -1.0f, 0.0f, Colors::GREEN.to_argb()},
  Vertex {-1.0f, -1.0f, 0.0f, Colors::BLUE.to_argb()},
};

constexpr array<Vertex, 4> QUAD_VERTICES {
  Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
  Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
  Vertex {1.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255u, 0, 0)},
  Vertex {-1.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255u, 0, 255u)},
};

} // namespace

FirstTriangle::FirstTriangle(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()} {
  const array vertexLayout {
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
  };

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = vertexLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.dithering = true;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  mQuadPipeline = mResourceCache.create_pipeline(pipelineDesc);

  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.fillMode = FillMode::Wireframe;
  mWireframePipeline = mResourceCache.create_pipeline(pipelineDesc);

  const span vertexData {as_bytes(span {TRIANGLE_VERTICES})};

  VertexBufferDescriptor vbDesc;
  vbDesc.layout = vertexLayout;
  vbDesc.sizeInBytes = 4 * sizeof(Vertex);

  mVertexBuffer = mResourceCache.create_vertex_buffer(vbDesc, vertexData);
}

FirstTriangle::~FirstTriangle() noexcept {
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mWireframePipeline);
  mResourceCache.destroy(mQuadPipeline);
  mResourceCache.destroy(mPipeline);
}

auto FirstTriangle::on_draw(const DrawContext& drawContext) -> void {
  CommandList cmdList;

  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f, 0);

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

  const f32 aspectRatio {static_cast<f32>(drawContext.viewport.width()) /
                         static_cast<f32>(drawContext.viewport.height())};

  cmdList.set_transform(
    TransformState::ViewToViewport,
    Matrix4x4f32::perspective_projection(90.0_deg, aspectRatio, 0.1f, 100.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(
    TransformState::ModelToWorld,
    Matrix4x4f32::scaling(mScale) * Matrix4x4f32::rotation_y(mRotationY) *
      Matrix4x4f32::translation(Vector3f32 {0.0f, 0.0f, 2.0f}));

  cmdList.bind_vertex_buffer(mVertexBuffer, 0);

  const u32 vertexCount {
    [this]() -> u32 { return mCurrentExercise == 2 ? 4 : 3; }()};
  cmdList.draw(0, vertexCount);

  drawContext.commandLists.emplace_back(std::move(cmdList));
}

auto FirstTriangle::on_tick(Engine& engine) -> void {
  const auto dt {static_cast<f32>(engine.delta_time())};

  // 90° per second
  mRotationY += Angle::degrees(90.0f * dt);
  while (mRotationY.radians() > PI) {
    mRotationY -= Angle::radians(PI * 2.0f);
  }

  if (ImGui::Begin("Settings##TribaseDreieck")) {
    const auto uploadDefaultTriangle {[this] {
      mResourceCache.with_mapping_of(mVertexBuffer, [](const span<byte> data) {
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

      mResourceCache.with_mapping_of(mVertexBuffer, [](const span<byte> data) {
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
      mTimeAccum += engine.delta_time();

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

      mResourceCache.with_mapping_of(mVertexBuffer, [&](const span<byte> data) {
        const span vertexData {as_bytes(span {triangleVertices})};
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });
    }

    if (mCurrentExercise == 4) {
      mTimeAccum += engine.delta_time();

      mScale = 1.0f + std::sin(PI / 2.0f * static_cast<f32>(mTimeAccum)) / 2.0f;
    }
  }

  ImGui::End();
}

} // namespace tribase
