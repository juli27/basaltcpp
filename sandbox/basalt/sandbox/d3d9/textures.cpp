#include <basalt/sandbox/d3d9/textures.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/mat4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui/imgui.h>

#include <algorithm> // for copy
#include <array>
#include <cstddef>
#include <utility> // for move

using std::array;
using std::byte;

using gsl::span;

using basalt::Angle;
using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::DepthTestPass;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureCoordinateSource;
using basalt::gfx::TextureTransformMode;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

constexpr u32 VERTEX_COUNT {2u * 50u};

}

Textures::Textures(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()}
  , mCamera {create_default_camera()} {
  TextureBlendingStage textureBlendingStage {};

  PipelineDescriptor pipelineDesc {};
  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  pipelineDesc.textureStages = span {&textureBlendingStage, 1};
  pipelineDesc.depthTest = DepthTestPass::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  textureBlendingStage.texCoordinateSrc =
    TextureCoordinateSource::VertexPositionInView;
  textureBlendingStage.texCoordinateTransformMode =
    TextureTransformMode::Count4;
  textureBlendingStage.texCoordinateProjected = true;

  mPipelineTci = mResourceCache.create_pipeline(pipelineDesc);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
    f32 tu {};
    f32 tv {};
  };

  array<Vertex, VERTEX_COUNT> vertices {};
  for (uSize i {0u}; i < 50u; i++) {
    const Angle theta {Angle::radians(2.0f * PI * i / (50 - 1))};
    const f32 sinTheta {theta.sin()};
    const f32 cosTheta {theta.cos()};

    Vertex& vertex1 {vertices[2 * i]};
    vertex1.x = sinTheta;
    vertex1.y = -1.0f;
    vertex1.z = cosTheta;
    vertex1.color = Colors::WHITE.to_argb();
    vertex1.tu = static_cast<f32>(i) / (50.0f - 1.0f);
    vertex1.tv = 1.0f;

    Vertex& vertex2 {vertices[2 * i + 1]};
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.color = ColorEncoding::pack_a8r8g8b8_u32(0x80, 0x80, 0x80);
    vertex2.tu = vertex1.tu;
    vertex2.tv = 0.0f;
  }

  const auto vertexData {as_bytes(span {vertices})};

  const VertexBufferDescriptor vertexBufferDesc {
    vertexData.size_bytes(),
    VertexLayout {
      VertexElement::Position3F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
      VertexElement::TextureCoords2F32,
    },
  };
  mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
  mResourceCache.with_mapping_of(mVertexBuffer, [&](const span<byte> mapping) {
    copy(vertexData.begin(), vertexData.end(), mapping.begin());
  });

  mSampler = mResourceCache.create_sampler(SamplerDescriptor {});
  mTexture = mResourceCache.load_texture("data/banana.bmp");
}

Textures::~Textures() noexcept {
  mResourceCache.destroy(mTexture);
  mResourceCache.destroy(mSampler);
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mPipeline);
}

auto Textures::on_draw(const DrawContext& context) -> void {
  CommandList cmdList {};

  cmdList.clear_attachments(
    Attachments {Attachment::Color, Attachment::ZBuffer}, Colors::BLUE, 1.0f,
    0);

  cmdList.bind_pipeline(mShowTci ? mPipelineTci : mPipeline);

  const Mat4f32 viewToViewport {mCamera.projection_matrix(context.viewport)};
  cmdList.set_transform(TransformState::ViewToViewport, viewToViewport);
  cmdList.set_transform(TransformState::WorldToView, mCamera.view_matrix());

  cmdList.set_transform(TransformState::ModelToWorld,
                        Mat4f32::rotation_x(mRotationX));

  if (mShowTci) {
    const Mat4f32 texTransform {
      viewToViewport * Mat4f32::scaling(Vector3f32 {0.5f, -0.5f, 1.0f}) *
      Mat4f32::translation(Vector3f32 {0.5f, 0.5f, 0.0f})};
    cmdList.set_transform(TransformState::Texture, texTransform);
  }

  cmdList.bind_sampler(mSampler);
  cmdList.bind_texture(mTexture);
  cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

  cmdList.draw(0, VERTEX_COUNT);

  context.commandLists.push_back(std::move(cmdList));
}

void Textures::on_tick(Engine& engine) {
  const auto dt {static_cast<f32>(engine.delta_time())};

  mRotationX += Angle::radians(dt);
  while (mRotationX.radians() > PI) {
    mRotationX -= Angle::radians(PI * 2.0f);
  }

  if (ImGui::Begin("Settings##D3D9")) {
    ImGui::Checkbox("Show TCI", &mShowTci);
  }

  ImGui::End();
}

} // namespace d3d9
