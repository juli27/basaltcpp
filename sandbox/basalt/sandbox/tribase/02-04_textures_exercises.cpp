#include "tribase_examples.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

using namespace basalt;
using namespace basalt::literals;
using namespace std::literals;

namespace {

auto constexpr TEXTURE_FILE_PATH = "data/banana.bmp"sv;

struct Vertex {
  Vector3f32 pos;
  Vector2f32 uv;

  static constexpr auto sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::TextureCoords2F32>();
};

auto constexpr TRIANGLE_VERTICES = std::array{
  Vertex{{0.0f, 0.5f, 0.0f}, {0.5f, 0.0f}},
  Vertex{{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
  Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
};

class TexturesExercises final : public basalt::View {
public:
  explicit TexturesExercises(basalt::Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mSampler{mGfxCache->create_sampler(gfx::SamplerCreateInfo{})}
    , mTexture{mGfxCache->load_texture_2d(TEXTURE_FILE_PATH)}
    , mVertexBuffer{[&] {
      auto const vertexData = as_bytes(gsl::span{TRIANGLE_VERTICES});
      auto info = gfx::VertexBufferCreateInfo{};
      info.sizeInBytes = vertexData.size_bytes();
      info.layout = Vertex::sLayout;

      return mGfxCache->create_vertex_buffer(info, vertexData);
    }()}
    , mPipeline{[&] {
      auto fs = gfx::FixedFragmentShaderCreateInfo{};
      constexpr auto textureStages = std::array{gfx::TextureStage{}};
      fs.textureStages = textureStages;

      auto info = gfx::PipelineCreateInfo{};
      info.fragmentShader = &fs;
      info.vertexLayout = Vertex::sLayout;
      info.primitiveType = gfx::PrimitiveType::TriangleList;
      return mGfxCache->create_pipeline(info);
    }()} {
  }

private:
  gfx::ResourceCachePtr mGfxCache;
  gfx::SamplerHandle mSampler;
  gfx::TextureHandle mTexture;
  gfx::VertexBufferHandle mVertexBuffer;
  gfx::PipelineHandle mPipeline;
  i32 mUvAnimationMode{0};
  SecondsF32 mTime{0};
  bool mAnimateUvs{false};

  auto on_update(UpdateContext& ctx) -> void override {
    if (mAnimateUvs) {
      mTime += ctx.deltaTime;
    }

    auto const& gfxCtx = ctx.engine.gfx_context();

    if (ImGui::Begin("Settings##TribaseTexturesEx")) {
      ImGui::Checkbox("Animate UVs", &mAnimateUvs);
      ImGui::SameLine();
      if (ImGui::Button("Reset")) {
        mTime = 0s;
        gfxCtx.with_mapping_of(
          mVertexBuffer, [](gsl::span<std::byte> const vbData) {
            auto const vertexData = as_bytes(gsl::span{TRIANGLE_VERTICES});

            std::copy_n(vertexData.begin(),
                        std::min(vertexData.size_bytes(), vbData.size_bytes()),
                        vbData.begin());
          });
      }

      ImGui::Indent();

      if (ImGui::RadioButton("Move", &mUvAnimationMode, 0)) {
        mTime = 0s;
      }

      if (ImGui::RadioButton("Rotate", &mUvAnimationMode, 1)) {
        mTime = 0s;
      }
    }

    ImGui::End();

    auto const t = mTime.count();

    if (mUvAnimationMode == 0) {
      gfxCtx.with_mapping_of(
        mVertexBuffer, [&](gsl::span<std::byte> const vbData) {
          auto const vertexData =
            gsl::span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                              vbData.size() / sizeof(Vertex)};

          auto const translateUv = [&](Vertex const& v) {
            return Vertex{v.pos, Vector2f32{
                                   v.uv.x() + 0.25f * t,
                                   v.uv.y(),
                                 }};
          };
          std::transform(TRIANGLE_VERTICES.begin(), TRIANGLE_VERTICES.end(),
                         vertexData.begin(), translateUv);
        });
    } else if (mUvAnimationMode == 1) {
      gfxCtx.with_mapping_of(
        mVertexBuffer, [&](gsl::span<std::byte> const vbData) {
          auto const vertexData =
            gsl::span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                              vbData.size() / sizeof(Vertex)};

          auto const rotation =
            Matrix4x4f32::rotation_z(Angle::degrees(45.0f * t));
          auto const rotateUv = [&](Vertex const& v) {
            return Vertex{v.pos,
                          Vector2f32{
                            v.uv.dot(Vector2f32{rotation.m11, rotation.m21}),
                            v.uv.dot(Vector2f32{rotation.m12, rotation.m22}),
                          }};
          };
          std::transform(TRIANGLE_VERTICES.begin(), TRIANGLE_VERTICES.end(),
                         vertexData.begin(), rotateUv);
        });
    }

    auto cmdList = gfx::CommandList{};
    cmdList.clear_attachments(gfx::Attachments{gfx::Attachment::RenderTarget},
                              Color::from_non_linear(0.103f, 0.103f, 0.103f));
    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_sampler(0, mSampler);
    cmdList.bind_texture(0, mTexture);

    auto const& drawCtx = ctx.drawCtx;
    auto const aspectRatio = drawCtx.viewport.aspect_ratio();
    cmdList.set_transform(
      gfx::TransformState::ViewToClip,
      Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f));
    cmdList.set_transform(gfx::TransformState::WorldToView,
                          Matrix4x4f32::identity());
    cmdList.set_transform(gfx::TransformState::LocalToWorld,
                          Matrix4x4f32::translation(0.0f, 0.0f, 1.0f));
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, 3);

    drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

} // namespace

auto TribaseExamples::new_02_04_textures_exercises(Engine& engine) -> ViewPtr {
  return std::make_shared<TexturesExercises>(engine);
}
