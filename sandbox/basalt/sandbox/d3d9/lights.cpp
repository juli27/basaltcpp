#include <basalt/sandbox/d3d9/lights.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/mat4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <cmath>

using std::array;
using std::byte;
using std::string_view;
using namespace std::literals;

using gsl::span;

using basalt::DirectionalLight;
using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::RectangleU16;
using basalt::Size2Du16;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::Camera;
using basalt::gfx::CommandList;
using basalt::gfx::DepthTestPass;
using basalt::gfx::Drawable;
using basalt::gfx::Light;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCache;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBuffer;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

constexpr u32 VERTEX_COUNT {2u * 50u};

}

struct Lights::MyDrawable final : Drawable {
  explicit MyDrawable(Engine& engine) noexcept
    : mResourceCache {engine.gfx_resource_cache()} {
    PipelineDescriptor pipelineDesc {};
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.lighting = true;
    pipelineDesc.depthTest = DepthTestPass::IfLessEqual;
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
      const f32 theta {2.0f * PI * i / (50 - 1)};
      const f32 sinTheta {std::sin(theta)};
      const f32 cosTheta {std::cos(theta)};

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
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  MyDrawable(const MyDrawable&) = delete;
  MyDrawable(MyDrawable&&) noexcept = default;

  ~MyDrawable() noexcept override {
    mResourceCache.destroy_pipeline(mPipeline);
    mResourceCache.destroy_vertex_buffer(mVertexBuffer);
  }

  auto operator=(const MyDrawable&) -> MyDrawable& = delete;
  auto operator=(MyDrawable&&) -> MyDrawable& = delete;

  void tick(const Engine& engine) {
    const auto dt {static_cast<f32>(engine.delta_time())};

    mAngleXRad += 2.0f * dt;
    if (mAngleXRad > PI) {
      mAngleXRad -= PI * 2.0f;
    }

    mLightAngle += 20.0f / 7.0f * dt;
    // reset when rotated 360°
    while (mLightAngle >= PI * 2.0f) {
      mLightAngle -= PI * 2.0f;
    }
  }

  auto draw(ResourceCache&, const Size2Du16 viewport, const RectangleU16&)
    -> std::tuple<CommandList, RectangleU16> override {
    CommandList cmdList {};

    cmdList.clear_attachments(
      Attachments {Attachment::Color, Attachment::ZBuffer}, Colors::BLUE, 1.0f,
      0);

    cmdList.bind_pipeline(mPipeline);

    cmdList.set_transform(TransformState::ViewToViewport,
                          mCamera.projection_matrix(viewport));

    cmdList.set_transform(TransformState::WorldToView, mCamera.view_matrix());

    const Light light {DirectionalLight {
      Vector3f32::normalize(
        Vector3f32 {std::cos(mLightAngle), 1.0f, std::sin(mLightAngle)}),
      Color {1.0f, 1.0f, 1.0f, 0.0f},
      Color {},
    }};

    cmdList.set_lights(span {&light, 1});

    cmdList.set_ambient_light(Color::from_rgba(0x20, 0x20, 0x20, 0));

    cmdList.set_material(Colors::YELLOW, Colors::YELLOW, Color {});
    cmdList.set_transform(TransformState::ModelToWorld,
                          Mat4f32::rotation_x(mAngleXRad));

    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

    cmdList.draw(0, VERTEX_COUNT);

    return {std::move(cmdList), viewport.to_rectangle()};
  }

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};
  Camera mCamera {create_default_camera()};
  f32 mAngleXRad {};
  f32 mLightAngle {};
};

Lights::Lights(Engine& engine)
  : mDrawable {std::make_shared<MyDrawable>(engine)} {
}

auto Lights::name() -> string_view {
  return "Tutorial 4: Creating and Using Lights"sv;
}

auto Lights::drawable() -> basalt::gfx::DrawablePtr {
  return mDrawable;
}

void Lights::tick(Engine& engine) {
  mDrawable->tick(engine);
}

} // namespace d3d9
