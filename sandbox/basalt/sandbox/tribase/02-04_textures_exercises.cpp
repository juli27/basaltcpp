#include <basalt/sandbox/tribase/02-04_textures_exercises.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
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
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace tribase {

namespace {

constexpr auto TEXTURE_FILE_PATH = "data/banana.bmp"sv;

struct Vertex final {
  Vector3f32 pos{};
  Vector2f32 uv{};

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::TextureCoords2F32>();
};

constexpr auto TRIANGLE_VERTICES = array{
  Vertex{{0.0f, 0.5f, 0.0f}, {0.5f, 0.0f}},
  Vertex{{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
  Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
};

} // namespace

TexturesExercises::TexturesExercises(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mSampler{mGfxCache->create_sampler({})}
  , mTexture{mGfxCache->load_texture_2d(TEXTURE_FILE_PATH)}
  , mVertexBuffer{[&] {
    auto const vertexData = as_bytes(span{TRIANGLE_VERTICES});

    return mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
  }()}
  , mPipeline{[&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto desc = PipelineCreateInfo{};
    desc.fragmentShader = &fs;
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    return mGfxCache->create_pipeline(desc);
  }()} {
}

auto TexturesExercises::on_update(UpdateContext& ctx) -> void {
  auto const& gfxCtx = ctx.engine.gfx_context();

  if (ImGui::Begin("Settings##TribaseTexturesEx")) {
    auto uploadTriangle = [&] {
      gfxCtx.with_mapping_of(mVertexBuffer, [](span<byte> const vbData) {
        auto const vertexData = as_bytes(span{TRIANGLE_VERTICES});

        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), vbData.size_bytes()),
                    vbData.begin());
      });
    };

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

  auto const dt = ctx.deltaTime.count();

  if (mCurrentExercise == 1) {
    gfxCtx.with_mapping_of(mVertexBuffer, [&](span<byte> const vbData) {
      auto const vertexData =
        span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                     vbData.size() / sizeof(Vertex)};

      // READING from the vertex buffer!
      for (auto& vertex : vertexData) {
        vertex.uv.x() += 0.25f * dt;
      }
    });
  }

  if (mCurrentExercise == 2) {
    gfxCtx.with_mapping_of(mVertexBuffer, [&](span<byte> const vbData) {
      auto const vertexData =
        span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                     vbData.size() / sizeof(Vertex)};

      auto const rotation =
        Matrix4x4f32::rotation_z(Angle::degrees(45.0f * dt));
      // READING from the vertex buffer!
      for (auto& [pos, uv] : vertexData) {
        uv = {uv.dot(Vector2f32{rotation.m11, rotation.m21}),
              uv.dot(Vector2f32{rotation.m12, rotation.m22})};
      }
    });
  }

  auto cmdList = CommandList{};
  cmdList.clear_attachments(Attachments{Attachment::RenderTarget},
                            Color::from_non_linear(0.103f, 0.103f, 0.103f));
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mTexture);

  auto const& drawCtx = ctx.drawCtx;
  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::translation(0.0f, 0.0f, 1.0f));
  cmdList.bind_vertex_buffer(mVertexBuffer);
  cmdList.draw(0, 3);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
