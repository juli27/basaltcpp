#include <basalt/sandbox/d3d9/lights.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

using std::array;
using std::byte;

using gsl::span;

using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::DirectionalLight;
using basalt::gfx::Light;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TestOp;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

constexpr u32 VERTEX_COUNT {2u * 50u};

}

Lights::Lights(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()}
  , mCamera {create_default_camera()} {
  constexpr array<const VertexElement, 2> vertexLayout {
    VertexElement::Position3F32,
    VertexElement::Normal3F32,
  };

  PipelineDescriptor pipelineDesc {};
  pipelineDesc.vertexInputState = vertexLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  pipelineDesc.lighting = true;
  pipelineDesc.depthTest = TestOp::PassIfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    f32 nx {};
    f32 ny {};
    f32 nz {};
  };

  array<Vertex, VERTEX_COUNT> vertices {};
  for (uSize i = 0u; i < 50u; i++) {
    const Angle theta {Angle::radians(2.0f * PI * i / (50 - 1))};
    const f32 sinTheta {theta.sin()};
    const f32 cosTheta {theta.cos()};

    auto& vertex1 = vertices[2 * i];
    vertex1.x = sinTheta;
    vertex1.y = -1.0f;
    vertex1.z = cosTheta;
    vertex1.nx = sinTheta;
    vertex1.nz = cosTheta;

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.nx = sinTheta;
    vertex2.nz = cosTheta;
  }

  const auto vertexData {as_bytes(span {vertices})};

  const VertexBufferDescriptor vertexBufferDesc {
    vertexData.size_bytes(),
    VertexLayout {
      VertexElement::Position3F32,
      VertexElement::Normal3F32,
    },
  };
  mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
  mResourceCache.with_mapping_of(mVertexBuffer, [&](const span<byte> mapping) {
    std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
  });
}

Lights::~Lights() noexcept {
  mResourceCache.destroy(mPipeline);
  mResourceCache.destroy(mVertexBuffer);
}

auto Lights::on_draw(const DrawContext& context) -> void {
  CommandList cmdList {};

  cmdList.clear_attachments(
    Attachments {Attachment::Color, Attachment::ZBuffer}, Colors::BLUE, 1.0f,
    0);

  cmdList.bind_pipeline(mPipeline);

  cmdList.set_transform(TransformState::ViewToViewport,
                        mCamera.view_to_viewport(context.viewport));

  cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());

  const Light light {DirectionalLight {
    Vector3f32::normalize(
      Vector3f32 {mLightRotation.cos(), 1.0f, mLightRotation.sin()}),
    Color::from_non_linear(1.0f, 1.0f, 1.0f, 0.0f),
    Color {},
  }};

  cmdList.set_lights(span {&light, 1});

  cmdList.set_ambient_light(Color::from_non_linear_rgba8(0x20, 0x20, 0x20, 0));

  cmdList.set_material(Colors::YELLOW, Colors::YELLOW, Color {});
  cmdList.set_transform(TransformState::ModelToWorld,
                        Matrix4x4f32::rotation_x(mRotationX));

  cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

  cmdList.draw(0, VERTEX_COUNT);

  context.commandLists.push_back(std::move(cmdList));
}

void Lights::on_tick(Engine& engine) {
  const auto dt {static_cast<f32>(engine.delta_time())};

  constexpr f32 twoPi {2.0f * PI};

  mRotationX += Angle::radians(2.0f * dt);
  while (mRotationX.radians() > PI) {
    mRotationX -= Angle::radians(twoPi);
  }

  mLightRotation += Angle::radians(20.0f / 7.0f * dt);
  // reset to -180° when rotated more than 180°
  while (mLightRotation.radians() > PI) {
    mLightRotation -= Angle::radians(twoPi);
  }
}

} // namespace d3d9
