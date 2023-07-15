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
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureAddressMode;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStageArgument;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace tribase {

namespace {

constexpr i32 NUM_TRIANGLES {1024};

struct Vertex final {
  array<f32, 3> pos {};
  ColorEncoding::A8R8G8B8 color {};
  array<f32, 2> uv {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8,
                                  VertexElement::TextureCoords2F32};
};

using Distribution = uniform_real_distribution<float>;

} // namespace

struct Textures::TriangleData final {
  Vector3f32 position;
  Vector3f32 rotation;
  f32 scale {1.0f};
  Vector3f32 velocity;
  Vector3f32 rotationVelocity;
};

Textures::Textures(Engine& engine)
  : mTriangles {NUM_TRIANGLES}
  , mGfxCache {engine.create_gfx_resource_cache()}
  , mTexture {mGfxCache->load_texture("data/tribase/Texture.bmp")} {
  TextureBlendingStage textureStage;
  textureStage.arg1 = TextureStageArgument::SampledTexture;
  textureStage.arg2 = TextureStageArgument::Diffuse;
  textureStage.colorOp = TextureOp::Modulate;
  textureStage.alphaOp = TextureOp::SelectArg1;
  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipeline = mGfxCache->create_pipeline(pipelineDesc);

  default_random_engine randomEngine {random_device {}()};
  Distribution scaleRng {1.0f, 5.0f};
  Distribution rng2 {-1.0f, 1.0f};
  Distribution rng3 {0.1f, 5.0f};
  Distribution rng4 {0.0f, 1.0f};
  Distribution rng5 {-1.0f, 2.0f};

  const auto normalizedRandomVector {[&] {
    return Vector3f32::normalize(rng2(randomEngine), rng2(randomEngine),
                                 rng2(randomEngine));
  }};

  vector<Vertex> vertices {uSize {3} * NUM_TRIANGLES};
  const Vector3f32 startPos {0.0f, 0.0f, 50.0f};

  for (uSize i {0}; i < NUM_TRIANGLES; ++i) {
    auto& triangle {mTriangles[i]};

    triangle.position = startPos;
    triangle.scale = scaleRng(randomEngine);

    const Vector3f32 direction {normalizedRandomVector()};
    const Vector3f32 velocity {direction * rng3(randomEngine)};
    const Vector3f32 rotationVelocity {rng2(randomEngine), rng2(randomEngine),
                                       rng2(randomEngine)};
    triangle.velocity = velocity;
    triangle.rotationVelocity = rotationVelocity;

    for (uSize j {0}; j < 3; ++j) {
      auto& vertex {vertices[j + 3 * i]};

      const auto pos {normalizedRandomVector()};
      vertex.pos = pos.elements;
      vertex.color =
        Color::from_non_linear(rng4(randomEngine), rng4(randomEngine),
                               rng4(randomEngine))
          .to_argb();
      vertex.uv = {rng5(randomEngine), rng5(randomEngine)};
    }
  }

  const span vertexData {as_bytes(span {vertices})};
  mVertexBuffer = mGfxCache->create_vertex_buffer(
    {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

  SamplerDescriptor samplerDesc;
  samplerDesc.magFilter = TextureFilter::Point;
  samplerDesc.minFilter = TextureFilter::Point;
  samplerDesc.mipFilter = TextureMipFilter::None;
  samplerDesc.addressModeU = TextureAddressMode::Repeat;
  samplerDesc.addressModeV = TextureAddressMode::Repeat;
  mSamplerPoint = mGfxCache->create_sampler(samplerDesc);

  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSamplerLinearWithMip = mGfxCache->create_sampler(samplerDesc);

  const auto& gfxInfo {engine.gfx_info()};
  BASALT_ASSERT(gfxInfo.currentDeviceCaps.samplerMinFilterAnisotropic);

  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Anisotropic;
  samplerDesc.mipFilter = TextureMipFilter::None;

  samplerDesc.maxAnisotropy = gfxInfo.currentDeviceCaps.samplerMaxAnisotropy;
  mSamplerAnisotropic = mGfxCache->create_sampler(samplerDesc);
}

Textures::~Textures() noexcept {
  mGfxCache->destroy(mSamplerAnisotropic);
  mGfxCache->destroy(mSamplerLinearWithMip);
  mGfxCache->destroy(mSamplerPoint);
  mGfxCache->destroy(mTexture);
  mGfxCache->destroy(mVertexBuffer);
  mGfxCache->destroy(mPipeline);
}

auto Textures::on_update(UpdateContext& ctx) -> void {
  const f32 dt {ctx.deltaTime.count()};
  mTimeAccum += dt;

  if (!is_key_down(Key::Space)) {
    for (uSize i {0}; i < NUM_TRIANGLES; ++i) {
      auto& triangle {mTriangles[i]};

      triangle.position += triangle.velocity * dt;
      triangle.rotation += triangle.rotationVelocity * dt;

      if (triangle.position.length() > 100.0f) {
        triangle.velocity *= -1.0f;
      }
    }
  }

  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_vertex_buffer(mVertexBuffer);
  cmdList.bind_texture(mTexture);

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

  const DrawContext& drawCtx {ctx.drawCtx};
  const f32 aspectRatio {drawCtx.viewport.aspect_ratio()};

  const auto viewToClip {
    Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f)};
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);

  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  for (u32 i {0}; i < NUM_TRIANGLES; ++i) {
    const auto& triangle {mTriangles[i]};

    const auto localToWorld {
      Matrix4x4f32::scaling(triangle.scale) *
      Matrix4x4f32::rotation_x(Angle::radians(triangle.rotation.x())) *
      Matrix4x4f32::rotation_y(Angle::radians(triangle.rotation.y())) *
      Matrix4x4f32::rotation_z(Angle::radians(triangle.rotation.z())) *
      Matrix4x4f32::translation(triangle.position)};
    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);

    cmdList.draw(3 * i, 3);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto Textures::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
