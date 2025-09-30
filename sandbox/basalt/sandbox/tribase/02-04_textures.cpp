#include "tribase_examples.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/resources.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/asserts.h>

#include <gsl/span>
#include <imgui.h>

#include <array>
#include <random>
#include <string_view>
#include <utility>
#include <vector>

using namespace basalt;
using namespace basalt::literals;
using namespace std::literals;

namespace {

auto constexpr TEXTURE_FILE_PATH = "data/tribase/Texture.bmp"sv;

auto constexpr NUM_TRIANGLES = uSize{1024};

struct Triangle {
  Vector3f32 position;
  Vector3f32 rotation;
  f32 scale{1.0f};
  Vector3f32 velocity;
  Vector3f32 rotationVelocity;
};

struct Vertex {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 color{};
  Vector2f32 uv;

  static auto constexpr sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8,
                            gfx::VertexElement::TextureCoords2F32>();
};

using Distribution = std::uniform_real_distribution<float>;

class Textures final : public View {
public:
  explicit Textures(Engine& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mTexture{mGfxCache->load_texture_2d(TEXTURE_FILE_PATH)}
    , mPipeline{[&] {
      auto fsInfo = gfx::FixedFragmentShaderCreateInfo{};
      auto constexpr textureStages = std::array{gfx::TextureStage{}};
      fsInfo.textureStages = textureStages;

      auto info = gfx::PipelineCreateInfo{};
      info.fragmentShader = &fsInfo;
      info.vertexLayout = Vertex::sLayout;
      info.primitiveType = gfx::PrimitiveType::TriangleList;
      info.depthTest = gfx::TestPassCond::IfLessEqual;
      info.depthWriteEnable = true;

      return mGfxCache->create_pipeline(info);
    }()} {
    update_sampler(engine.gfx_context());

    auto randomEngine = std::default_random_engine{std::random_device{}()};

    auto rng2 = Distribution{-1.0f, 1.0f};
    auto const getRandomNormalizedVector = [&] {
      return Vector3f32::normalized(rng2(randomEngine), rng2(randomEngine),
                                    rng2(randomEngine));
    };

    {
      auto scaleRng = Distribution{1.0f, 5.0f};
      auto rng3 = Distribution{0.1f, 5.0f};
      auto constexpr position = Vector3f32{0.0f, 0.0f, 50.0f};
      auto constexpr rotation = Vector3f32{};
      mTriangles.reserve(NUM_TRIANGLES);
      for (auto i = uSize{0}; i < NUM_TRIANGLES; ++i) {
        auto const scale = scaleRng(randomEngine);
        auto const velocity = rng3(randomEngine) * getRandomNormalizedVector();
        auto const rotationVelocity = Vector3f32{
          rng2(randomEngine), rng2(randomEngine), rng2(randomEngine)};

        mTriangles.push_back(
          Triangle{position, rotation, scale, velocity, rotationVelocity});
      }
    }

    auto const vertices = [&] {
      auto rng4 = Distribution{0.0f, 1.0f};
      auto const getRandomColor = [&] {
        return Color::from_non_linear(rng4(randomEngine), rng4(randomEngine),
                                      rng4(randomEngine));
      };
      auto rng5 = Distribution{-1.0f, 2.0f};
      auto const getRandomUv = [&] {
        return Vector2f32{rng5(randomEngine), rng5(randomEngine)};
      };

      auto constexpr numVertices = 3 * NUM_TRIANGLES;
      auto vertices = std::vector<Vertex>{};
      vertices.reserve(numVertices);
      for (auto i = uSize{0}; i < NUM_TRIANGLES; ++i) {
        for (auto vertIdx = uSize{0}; vertIdx < 3; ++vertIdx) {
          vertices.push_back(Vertex{
            getRandomNormalizedVector(),
            getRandomColor().to_argb(),
            getRandomUv(),
          });
        }
      }

      return vertices;
    }();

    auto const vertexData = as_bytes(gsl::span{vertices});
    mVertexBuffer = mGfxCache->create_vertex_buffer(
      gfx::VertexBufferCreateInfo{vertexData.size_bytes(), Vertex::sLayout},
      vertexData);
  }

private:
  std::vector<Triangle> mTriangles;
  gfx::ResourceCachePtr mGfxCache;
  gfx::Sampler mSampler;
  gfx::TextureHandle mTexture;
  gfx::VertexBufferHandle mVertexBuffer;
  gfx::PipelineHandle mPipeline;
  gfx::TextureFilter mMagFilter{gfx::TextureFilter::Bilinear};
  gfx::TextureFilter mMinFilter{gfx::TextureFilter::Bilinear};
  gfx::TextureMipFilter mMipFilter{gfx::TextureMipFilter::Linear};
  bool mIsPaused{false};

  auto update_sampler(gfx::Context& gfxCtx) -> void {
    auto info = gfx::SamplerCreateInfo{mMagFilter, mMinFilter, mMipFilter};

    auto const& gfxInfo = gfxCtx.gfx_info();
    auto const& caps = gfxInfo.adapterInfos[gfxInfo.currentAdapter].deviceCaps;
    BASALT_ASSERT(caps.samplerMinFilterAnisotropic);
    info.maxAnisotropy = caps.samplerMaxAnisotropy;

    mSampler = gfxCtx.create_sampler(info);
  }

  auto on_update(UpdateContext& ctx) -> void override {
    if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
      mIsPaused = !mIsPaused;
    }

    if (ImGui::Begin("Settings##TribaseTextures")) {
      ImGui::TextUnformatted(mIsPaused ? "Press SPACE to unpause"
                                       : "Press SPACE to pause");

      auto const editTextureFilter = [&](gfx::TextureFilter& filter) -> bool {
        auto value = static_cast<int>(filter);

        auto didInteract = ImGui::RadioButton("Point", &value, 0);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Bilinear", &value, 1);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Anisotropic", &value, 2);

        filter = static_cast<gfx::TextureFilter>(value);

        return didInteract;
      };

      auto const editTextureMipFilter =
        [&](gfx::TextureMipFilter& filter) -> bool {
        auto value = static_cast<int>(filter);

        auto didInteract = ImGui::RadioButton("None", &value, 0);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Point", &value, 1);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Linear", &value, 2);

        filter = static_cast<gfx::TextureMipFilter>(value);

        return didInteract;
      };

      auto& gfxCtx = ctx.engine.gfx_context();
      ImGui::SeparatorText("MAG");
      ImGui::PushID("magFilter");
      if (editTextureFilter(mMagFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

      ImGui::SeparatorText("MIN");
      ImGui::PushID("minFilter");
      if (editTextureFilter(mMinFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

      ImGui::SeparatorText("MIP");
      ImGui::PushID("mipFilter");
      if (editTextureMipFilter(mMipFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();
    }
    ImGui::End();

    auto const dt = mIsPaused ? 0.0f : ctx.deltaTime.count();
    for (auto& triangle : mTriangles) {
      triangle.position += triangle.velocity * dt;
      triangle.rotation += triangle.rotationVelocity * dt;

      if (triangle.position.length() > 100.0f) {
        triangle.velocity *= -1.0f;
      }
    }

    auto cmdList = gfx::CommandList{};
    cmdList.clear_attachments(gfx::Attachments{gfx::Attachment::RenderTarget,
                                               gfx::Attachment::DepthBuffer},
                              Color::from_non_linear_rgba8(0, 0, 63), 1.0f);
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

    cmdList.bind_vertex_buffer(mVertexBuffer);
    for (auto i = u32{0}; i < NUM_TRIANGLES; ++i) {
      auto const& triangle = mTriangles[i];

      cmdList.set_transform(
        gfx::TransformState::LocalToWorld,
        Matrix4x4f32::scaling(triangle.scale) *
          Matrix4x4f32::rotation_x(Angle::radians(triangle.rotation.x())) *
          Matrix4x4f32::rotation_y(Angle::radians(triangle.rotation.y())) *
          Matrix4x4f32::rotation_z(Angle::radians(triangle.rotation.z())) *
          Matrix4x4f32::translation(triangle.position));
      cmdList.draw(3 * i, 3);
    }

    drawCtx.commandLists.emplace_back(std::move(cmdList));
  }

  auto on_input(InputEvent const&) -> InputEventHandled override {
    return InputEventHandled::Yes;
  }
};

} // namespace

auto TribaseExamples::new_02_04_textures_example(Engine& engine) -> ViewPtr {
  return std::make_shared<Textures>(engine);
}
