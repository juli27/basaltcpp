#include <basalt/sandbox/tribase/02-04_textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/asserts.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <array>
#include <random>
#include <utility>

using std::array;
using std::default_random_engine;
using std::random_device;
using std::uniform_real_distribution;
using std::vector;

using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::Key;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestOp;
using basalt::gfx::TextureAddressMode;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStageArgument;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;

namespace tribase {

namespace {

constexpr i32 NUM_TRIANGLES {1024};

struct Vertex final {
  f32 x;
  f32 y;
  f32 z;
  ColorEncoding::A8R8G8B8 color;
  f32 u;
  f32 v;
};

using Distribution = uniform_real_distribution<float>;

} // namespace

struct Textures::TriangleData final {
  Vector3f32 position {0.0f};
  Vector3f32 rotation {0.0f};
  f32 scale {1.0f};
  Vector3f32 velocity {0.0f};
  Vector3f32 rotationVelocity {0.0f};
};

Textures::Textures(Engine& engine)
  : mTriangles {NUM_TRIANGLES}, mResourceCache {engine.gfx_resource_cache()} {
  const array vertexLayout {
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
    VertexElement::TextureCoords2F32,
  };

  TextureBlendingStage textureStage;
  textureStage.arg1 = TextureStageArgument::SampledTexture;
  textureStage.arg2 = TextureStageArgument::Diffuse;
  textureStage.colorOp = TextureOp::Modulate;
  textureStage.alphaOp = TextureOp::SelectArg1;

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = vertexLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.depthTest = TestOp::PassIfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  default_random_engine randomEngine {random_device {}()};

  Distribution scaleRng {1.0f, 5.0f};
  Distribution rng2 {-1.0f, 1.0f};
  Distribution rng3 {0.1f, 5.0f};
  Distribution rng4 {0.0f, 1.0f};
  Distribution rng5 {-1.0f, 2.0f};

  const auto normalizedRandomVector {[&] {
    return Vector3f32::normalize(Vector3f32 {
      rng2(randomEngine),
      rng2(randomEngine),
      rng2(randomEngine),
    });
  }};

  vector<Vertex> vertices {uSize {3} * NUM_TRIANGLES};

  const Vector3f32 startPos {0.0f, 0.0f, 50.0f};

  for (uSize i {0}; i < NUM_TRIANGLES; ++i) {
    auto& triangle {mTriangles[i]};

    triangle.position = startPos;
    triangle.scale = scaleRng(randomEngine);

    const Vector3f32 direction {normalizedRandomVector()};
    const Vector3f32 velocity {direction * rng3(randomEngine)};
    const Vector3f32 rotationVelocity {
      rng2(randomEngine),
      rng2(randomEngine),
      rng2(randomEngine),
    };
    triangle.velocity = velocity;
    triangle.rotationVelocity = rotationVelocity;

    for (uSize j {0}; j < 3; ++j) {
      auto& vertex {vertices[j + 3 * i]};

      const auto pos {normalizedRandomVector()};
      vertex.x = pos.x();
      vertex.y = pos.y();
      vertex.z = pos.z();
      vertex.color =
        Color::from_non_linear(rng4(randomEngine), rng4(randomEngine),
                               rng4(randomEngine))
          .to_argb();
      vertex.u = rng5(randomEngine);
      vertex.v = rng5(randomEngine);
    }
  }

  const span vertexData {as_bytes(span {vertices})};

  VertexBufferDescriptor vbDesc;
  vbDesc.layout = vertexLayout;
  vbDesc.sizeInBytes = vertexData.size_bytes();
  mVertexBuffer = mResourceCache.create_vertex_buffer(vbDesc, vertexData);

  mTexture = mResourceCache.load_texture("data/tribase/Texture.bmp");

  SamplerDescriptor samplerDesc;
  samplerDesc.magFilter = TextureFilter::Point;
  samplerDesc.minFilter = TextureFilter::Point;
  samplerDesc.mipFilter = TextureMipFilter::None;
  samplerDesc.addressModeU = TextureAddressMode::Repeat;
  samplerDesc.addressModeV = TextureAddressMode::Repeat;
  mSamplerPoint = mResourceCache.create_sampler(samplerDesc);

  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSamplerLinearWithMip = mResourceCache.create_sampler(samplerDesc);

  const auto& gfxInfo {engine.gfx_info()};
  BASALT_ASSERT(gfxInfo.currentDeviceCaps.samplerMinFilterAnisotropic);

  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Anisotropic;
  samplerDesc.mipFilter = TextureMipFilter::None;

  samplerDesc.maxAnisotropy = gfxInfo.currentDeviceCaps.samplerMaxAnisotropy;
  mSamplerAnisotropic = mResourceCache.create_sampler(samplerDesc);
}

Textures::~Textures() noexcept {
  mResourceCache.destroy(mSamplerAnisotropic);
  mResourceCache.destroy(mSamplerLinearWithMip);
  mResourceCache.destroy(mSamplerPoint);
  mResourceCache.destroy(mTexture);
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mPipeline);
}

auto Textures::on_update(UpdateContext& ctx) -> void {
  const Engine& engine {ctx.engine};

  mTimeAccum += engine.delta_time();

  if (!is_key_down(Key::Space)) {
    for (uSize i {0}; i < NUM_TRIANGLES; ++i) {
      const f32 deltaTime {static_cast<f32>(engine.delta_time())};

      auto& triangle {mTriangles[i]};

      triangle.position += triangle.velocity * deltaTime;
      triangle.rotation += triangle.rotationVelocity * deltaTime;

      if (triangle.position.length() > 100.0f) {
        triangle.velocity *= -1.0f;
      }
    }
  }

  CommandList cmdList;

  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f, 0);

  cmdList.bind_pipeline(mPipeline);

  const char* currentMode;

  if (static_cast<i32>(mTimeAccum / 3.0) % 3 == 0) {
    currentMode = "MIN: Linear, MAG: Linear, MIP: Linear";

    cmdList.bind_sampler(mSamplerLinearWithMip);
  } else if (static_cast<i32>(mTimeAccum / 3.0) % 3 == 2) {
    currentMode = "MIN: Point, MAG: Point, MIP: None";

    cmdList.bind_sampler(mSamplerPoint);
  } else {
    currentMode = "MIN: Anisotropic, MAG: Linear, MIP: None";

    cmdList.bind_sampler(mSamplerAnisotropic);
  }

  if (ImGui::Begin("Textures##TribaseTextures")) {
    ImGui::TextUnformatted("Hold SPACE to stop animating");
    ImGui::TextUnformatted(currentMode);
  }

  ImGui::End();

  cmdList.bind_texture(mTexture);

  const DrawContext& drawCtx {ctx.drawCtx};
  const f32 aspectRatio {static_cast<f32>(drawCtx.viewport.width()) /
                         static_cast<f32>(drawCtx.viewport.height())};

  cmdList.set_transform(
    TransformState::ViewToViewport,
    Matrix4x4f32::perspective_projection(90.0_deg, aspectRatio, 0.1f, 100.0f));

  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  cmdList.bind_vertex_buffer(mVertexBuffer, 0);

  for (u32 i {0}; i < NUM_TRIANGLES; ++i) {
    const auto& triangle {mTriangles[i]};

    cmdList.set_transform(
      TransformState::ModelToWorld,
      Matrix4x4f32::scaling(triangle.scale) *
        Matrix4x4f32::rotation_x(Angle::radians(triangle.rotation.x())) *
        Matrix4x4f32::rotation_y(Angle::radians(triangle.rotation.y())) *
        Matrix4x4f32::rotation_z(Angle::radians(triangle.rotation.z())) *
        Matrix4x4f32::translation(triangle.position));

    cmdList.draw(3 * i, 3);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto Textures::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
