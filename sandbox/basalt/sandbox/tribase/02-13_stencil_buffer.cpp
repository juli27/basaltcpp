#include <basalt/sandbox/tribase/02-13_stencil_buffer.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

namespace tribase {

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
using basalt::gfx::StencilOp;
using basalt::gfx::TestPassCond;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

struct Vertex {
  Vector3f32 pos {};
  ColorEncoding::A8R8G8B8 diffuse {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

constexpr auto THING_PATH {"data/tribase/Thing.x"sv};

} // namespace

StencilBuffer::StencilBuffer(const Engine& engine)
  : mGfxCache {engine.gfx_context().create_resource_cache()} {
  PipelineDescriptor pipelineDesc;
  pipelineDesc.depthTest = TestPassCond::Never;
  pipelineDesc.frontFaceStencilOp.test = TestPassCond::Always;
  pipelineDesc.frontFaceStencilOp.passDepthFailOp = StencilOp::IncrementClamp;
  pipelineDesc.backFaceStencilOp = pipelineDesc.frontFaceStencilOp;
  pipelineDesc.dithering = true;
  mPrePassPipeline = mGfxCache->create_pipeline(pipelineDesc);

  pipelineDesc.vertexLayout = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  pipelineDesc.depthTest = TestPassCond::Always;
  pipelineDesc.frontFaceStencilOp.test = TestPassCond::IfEqual;
  pipelineDesc.backFaceStencilOp.test = TestPassCond::IfEqual;
  mOverdrawPipeline1 = mGfxCache->create_pipeline(pipelineDesc);

  pipelineDesc.frontFaceStencilOp.test = TestPassCond::IfLessEqual;
  pipelineDesc.backFaceStencilOp.test = TestPassCond::IfLessEqual;
  mOverdrawPipeline2 = mGfxCache->create_pipeline(pipelineDesc);

  mThing = mGfxCache->load_x_model(THING_PATH);

  array rectsVertices {Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}},
                       Vertex {{-100, -100, 1}}, Vertex {{-100, 100, 1}},
                       Vertex {{100, -100, 1}},  Vertex {{100, 100, 1}}};
  for (uSize i {0}; i < 10; ++i) {
    const ColorEncoding::A8R8G8B8 color {
      Color::from_non_linear(static_cast<f32>(i) / 9.0f,
                             1.0f - static_cast<f32>(i) / 9.0f, 0)
        .to_argb()};
    rectsVertices[4 * i].diffuse = color;
    rectsVertices[4 * i + 1].diffuse = color;
    rectsVertices[4 * i + 2].diffuse = color;
    rectsVertices[4 * i + 3].diffuse = color;
  }

  const auto rectsVertexData {as_bytes(span {rectsVertices})};
  mRectanglesVb = mGfxCache->create_vertex_buffer(
    {rectsVertexData.size_bytes(), Vertex::sLayout}, rectsVertexData);
}

auto StencilBuffer::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  const f32 t {mTime.count()};

  CommandList cmdList;
  cmdList.clear_attachments(Attachments {Attachment::StencilBuffer}, {}, 0, 0);

  const auto& thingData {mGfxCache->get(mThing)};
  cmdList.bind_pipeline(mPrePassPipeline);
  cmdList.set_stencil_write_mask(~0u);
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(
      90_deg, ctx.drawCtx.viewport.aspect_ratio(), 0.1f, 100));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation(Angle::radians(t),
                                               Angle::radians(0.5f * t),
                                               Angle::radians(0.25f * t)) *
                          Matrix4x4f32::translation(0, 0, 3));
  XMeshCommandEncoder::draw_x_mesh(cmdList, thingData.meshes[0]);

  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation(Angle::radians(0.9f * t),
                                               Angle::radians(0.6f * t),
                                               Angle::radians(0.3f * t)) *
                          Matrix4x4f32::translation(-3, 0, 5));
  XMeshCommandEncoder::draw_x_mesh(cmdList, thingData.meshes[0]);

  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation(Angle::radians(1.1f * t),
                                               Angle::radians(0.4f * t),
                                               Angle::radians(0.35f * t)) *
                          Matrix4x4f32::translation(3, 0, 5));
  XMeshCommandEncoder::draw_x_mesh(cmdList, thingData.meshes[0]);

  cmdList.bind_pipeline(mOverdrawPipeline1);
  cmdList.bind_vertex_buffer(mRectanglesVb);
  cmdList.set_stencil_read_mask(~0u);
  cmdList.set_transform(TransformState::LocalToWorld, Matrix4x4f32::identity());
  for (u32 i {0}; i < 10; ++i) {
    if (i == 9) {
      cmdList.bind_pipeline(mOverdrawPipeline2);
    }
    cmdList.set_stencil_reference(i);
    cmdList.draw(4 * i, 4);
  }

  ctx.drawCtx.commandLists.push_back(std::move(cmdList));
}

} // namespace tribase
