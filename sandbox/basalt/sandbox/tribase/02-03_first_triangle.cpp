#include <basalt/api/view.h>

#include <basalt/sandbox/tribase/tribase_examples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <utility>

using namespace basalt;
using namespace basalt::literals;

namespace {

struct Vertex {
  Vector3f32 position;
  ColorEncoding::A8R8G8B8 color;

  static constexpr auto sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8>();
};

constexpr auto TRIANGLE_VERTICES = std::array{
  Vertex{{0.0f, 1.0f, 0.0f}, Colors::RED.to_argb()},
  Vertex{{1.0f, -1.0f, 0.0f}, Colors::GREEN.to_argb()},
  Vertex{{-1.0f, -1.0f, 0.0f}, Colors::BLUE.to_argb()},
};

constexpr auto QUAD_VERTICES = std::array{
  Vertex{{1.0f, -1.0f, 0.0f}, Colors::RED.to_argb()},
  Vertex{{-1.0f, -1.0f, 0.0f}, Colors::GREEN.to_argb()},
  Vertex{{1.0f, 1.0f, 0.0f}, Colors::BLUE.to_argb()},
  Vertex{{-1.0f, 1.0f, 0.0f}, 0xff00ffff_a8r8g8b8},
};

class FirstTriangle final : public View {
public:
  explicit FirstTriangle(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mVertexBuffer{[&] {
      auto const vertexData = as_bytes(gsl::span{QUAD_VERTICES});

      // need space for a quad
      constexpr auto vbSize = 4 * sizeof(Vertex);

      return mGfxCache->create_vertex_buffer({vbSize, Vertex::sLayout},
                                             vertexData);
    }()}
    , mSolidPipeline{[&] {
      auto desc = gfx::PipelineCreateInfo{};
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = gfx::PrimitiveType::TriangleStrip;

      return mGfxCache->create_pipeline(desc);
    }()}
    , mWireframePipeline{[&] {
      auto desc = gfx::PipelineCreateInfo{};
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = gfx::PrimitiveType::TriangleStrip;
      desc.fillMode = gfx::FillMode::Wireframe;

      return mGfxCache->create_pipeline(desc);
    }()} {
  }

private:
  gfx::ResourceCachePtr mGfxCache;
  gfx::VertexBufferHandle mVertexBuffer;
  gfx::PipelineHandle mSolidPipeline;
  gfx::PipelineHandle mWireframePipeline;
  Angle mRotationY;
  SecondsF32 mColorTime{0};
  SecondsF32 mScaleTime{0};
  f32 mScale{1.0f};
  bool mAnimateColors{false};
  bool mAnimateScale{false};
  bool mRenderWireFrame{false};

  auto on_update(UpdateContext& ctx) -> void override {
    auto const& gfxCtx = ctx.engine.gfx_context();

    if (ImGui::Begin("Settings##TribaseTriangle")) {
      if (ImGui::RadioButton("Render solid", !mRenderWireFrame)) {
        mRenderWireFrame = false;
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Render wireframe", mRenderWireFrame)) {
        mRenderWireFrame = true;
      }

      ImGui::Checkbox("Animate colors", &mAnimateColors);
      ImGui::SameLine();
      if (ImGui::Button("Reset colors")) {
        mColorTime = {};
      }

      ImGui::Checkbox("Animate scale", &mAnimateScale);
      ImGui::SameLine();
      if (ImGui::Button("Reset scale")) {
        mScaleTime = {};
      }
    }

    ImGui::End();

    auto const dt = ctx.deltaTime.count();

    // 90 deg per second
    mRotationY += Angle::degrees(90.0f * dt);

    if (mAnimateColors) {
      mColorTime += ctx.deltaTime;
    }
    auto const colorT = mColorTime.count();
    auto const value = std::cos(colorT) / 2.0f + 0.5f;

    auto vertices = QUAD_VERTICES;
    std::get<0>(vertices).color =
      Color::from_non_linear(value, 1.0f - value, 0.0f).to_argb();
    std::get<1>(vertices).color =
      Color::from_non_linear(0.0f, value, 1.0f - value).to_argb();
    std::get<2>(vertices).color =
      Color::from_non_linear(1.0f - value, 0.0f, value).to_argb();
    std::get<3>(vertices).color =
      Color::from_non_linear(1.0f - value, value, 1.0f).to_argb();

    gfxCtx.with_mapping_of(mVertexBuffer, [&](gsl::span<std::byte> const data) {
      auto const vertexData = as_bytes(gsl::span{vertices});
      std::copy_n(vertexData.begin(),
                  std::min(vertexData.size_bytes(), data.size_bytes()),
                  data.begin());
    });

    if (mAnimateScale) {
      mScaleTime += ctx.deltaTime;
    }

    auto const scaleT = mScaleTime.count();
    mScale = 1.0f + std::sin(PI / 2.0f * scaleT) / 2.0f;

    auto cmdList = gfx::CommandList{};
    cmdList.clear_attachments(gfx::Attachments{gfx::Attachment::RenderTarget,
                                               gfx::Attachment::DepthBuffer},
                              Color::from_non_linear_rgba8(0, 0, 63), 1.0f);

    cmdList.bind_pipeline(mRenderWireFrame ? mWireframePipeline
                                           : mSolidPipeline);

    auto const& drawCtx = ctx.drawCtx;

    auto const aspectRatio = drawCtx.viewport.aspect_ratio();
    cmdList.set_transform(
      gfx::TransformState::ViewToClip,
      Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f));

    cmdList.set_transform(gfx::TransformState::WorldToView,
                          Matrix4x4f32::identity());
    cmdList.set_transform(gfx::TransformState::LocalToWorld,
                          Matrix4x4f32::scaling(mScale) *
                            Matrix4x4f32::rotation_y(mRotationY) *
                            Matrix4x4f32::translation(0.0f, 0.0f, 2.0f));

    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, 4);

    drawCtx.commandLists.emplace_back(std::move(cmdList));
  }
};

} // namespace

auto TribaseExamples::new_first_triangle_example(Engine& engine) -> ViewPtr {
  return std::make_shared<FirstTriangle>(engine);
}
