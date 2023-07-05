#include <basalt/sandbox/tribase/02-04_textures_exercises.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

using namespace std::literals;

using std::array;
using std::byte;

using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace tribase {

namespace {

struct Vertex final {
  f32 x;
  f32 y;
  f32 z;
  f32 u;
  f32 v;
};

constexpr array TRIANGLE_VERTICES {
  Vertex {0.0f, 0.5f, 0.0f, 0.5f, 0.0f},
  Vertex {0.5f, -0.5f, 0.0f, 1.0f, 1.0f},
  Vertex {-0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
};

} // namespace

TexturesExercises::TexturesExercises(Engine& engine)
  : mGfxCache {engine.gfx_resource_cache()}
  , mTexture {mGfxCache.load_texture("data/banana.bmp"sv)} {
  constexpr array vertexLayout {
    VertexElement::Position3F32,
    VertexElement::TextureCoords2F32,
  };
  const span vertexData {as_bytes(span {TRIANGLE_VERTICES})};

  mVertexBuffer = mGfxCache.create_vertex_buffer(
    {
      vertexData.size(),
      vertexLayout,
    },
    vertexData);

  array vertexInputState {
    VertexElement::Position3F32,
    VertexElement::TextureCoords2F32,
  };

  TextureBlendingStage textureStage {};

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = vertexInputState;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;

  mPipeline = mGfxCache.create_pipeline(pipelineDesc);

  mSampler = mGfxCache.create_sampler({});
}

TexturesExercises::~TexturesExercises() noexcept {
  mGfxCache.destroy(mSampler);
  mGfxCache.destroy(mPipeline);
  mGfxCache.destroy(mVertexBuffer);
  mGfxCache.destroy(mTexture);
}

auto TexturesExercises::on_update(UpdateContext& ctx) -> void {
  const Engine& engine {ctx.engine};

  if (ImGui::Begin("Settings##TribaseTexturesEx")) {
    auto uploadTriangle {[this] {
      mGfxCache.with_mapping_of(mVertexBuffer, [](const span<byte> vbData) {
        const span vertexData {as_bytes(span {TRIANGLE_VERTICES})};

        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), vbData.size_bytes()),
                    vbData.begin());
      });
    }};

    if (ImGui::RadioButton("Exercise 1", &mCurrentExercise, 0)) {
      uploadTriangle();
    }

    if (ImGui::RadioButton("Exercise 2", &mCurrentExercise, 1)) {
      uploadTriangle();
    }

    if (ImGui::RadioButton("Exercise 3", &mCurrentExercise, 2)) {
      uploadTriangle();
    }
  }

  ImGui::End();

  const f32 deltaTime {static_cast<f32>(engine.delta_time())};

  if (mCurrentExercise == 1) {
    mGfxCache.with_mapping_of(mVertexBuffer, [&](const span<byte> vbData) {
      const span<Vertex> vertexData {reinterpret_cast<Vertex*>(vbData.data()),
                                     vbData.size() / sizeof(Vertex)};

      // READING from the vertex buffer!
      for (Vertex& vertex : vertexData) {
        vertex.u += 0.25f * deltaTime;
      }
    });
  }

  if (mCurrentExercise == 2) {
    mGfxCache.with_mapping_of(mVertexBuffer, [&](const span<byte> vbData) {
      const span<Vertex> vertexData {reinterpret_cast<Vertex*>(vbData.data()),
                                     vbData.size() / sizeof(Vertex)};

      const Matrix4x4f32 rotation {
        Matrix4x4f32::rotation_z(Angle::degrees(45.0f * deltaTime))};
      // READING from the vertex buffer!
      for (Vertex& vertex : vertexData) {
        Vector3f32 uv {vertex.u, vertex.v, 0.0f};
        vertex.u =
          Vector3f32::dot(uv, Vector3f32 {rotation.m11, rotation.m21, 0});
        vertex.v =
          Vector3f32::dot(uv, Vector3f32 {rotation.m12, rotation.m22, 0});
      }
    });
  }

  CommandList cmdList;
  cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                            Color::from_non_linear(0.103f, 0.103f, 0.103f),
                            1.0f, 0u);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_vertex_buffer(mVertexBuffer, 0);

  const DrawContext& drawCtx {ctx.drawCtx};
  const f32 aspectRatio {static_cast<f32>(drawCtx.viewport.width()) /
                         static_cast<f32>(drawCtx.viewport.height())};
  cmdList.set_transform(
    TransformState::ViewToViewport,
    Matrix4x4f32::perspective_projection(90.0_deg, aspectRatio, 0.1f, 100.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(
    TransformState::ModelToWorld,
    Matrix4x4f32::translation(Vector3f32 {0.0f, 0.0f, 1.0f}));

  cmdList.bind_sampler(mSampler);
  cmdList.bind_texture(mTexture);

  cmdList.draw(0, 3);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
