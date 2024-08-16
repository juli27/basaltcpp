#include <basalt/sandbox/tribase/02-10_volume_textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

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
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::BlendFactor;
using basalt::gfx::CommandList;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace {

constexpr auto TEXTURE_FILE_PATH = "data/tribase/Explosion.dds"sv;

struct Vertex {
  Vector3f32 pos;
  Vector3f32 tex1;

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::TextureCoords3F32>();
};

} // namespace

VolumeTextures::VolumeTextures(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mPipeline{[&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto desc = PipelineCreateInfo{};
    desc.fragmentShader = &fs;
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleStrip;
    desc.dithering = true;
    desc.srcBlendFactor = BlendFactor::SrcColor;
    desc.destBlendFactor = BlendFactor::OneMinusSrcColor;

    return mGfxCache->create_pipeline(desc);
  }()}
  , mSampler{mGfxCache->create_sampler(
      {TextureFilter::Bilinear, TextureFilter::Bilinear})}
  , mExplosionTexture{mGfxCache->load_texture_3d(TEXTURE_FILE_PATH)}
  , mVertexBuffer{[&] {
    constexpr auto vertices = array{
      Vertex{{-1, -1, 1}, {0, 1, 0}},
      Vertex{{-1, 1, 1}, {0, 0, 0}},
      Vertex{{1, -1, 1}, {1, 1, 0}},
      Vertex{{1, 1, 1}, {1, 0, 0}},
    };

    auto const vertexData = as_bytes(span{vertices});

    return mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
  }()} {
}

auto VolumeTextures::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();
  auto const& gfxCtx = ctx.engine.gfx_context();

  gfxCtx.with_mapping_of(mVertexBuffer, [&](span<byte> const vbData) {
    auto const vertexData =
      span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                   vbData.size_bytes() / sizeof(Vertex)};

    for (auto& vertex : vertexData) {
      vertex.tex1.z() = 0.25f * t;
    }
  });

  auto const& drawCtx = ctx.drawCtx;
  auto cmdList = CommandList{};
  cmdList.clear_attachments(Attachments{Attachment::RenderTarget},
                            Color::from_non_linear_rgba8(0, 0, 63));
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mExplosionTexture);
  cmdList.bind_vertex_buffer(mVertexBuffer);

  cmdList.set_transform(TransformState::ViewToClip,
                        Matrix4x4f32::perspective_projection(
                          90_deg, drawCtx.viewport.aspect_ratio(), 0.1f, 100));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(TransformState::LocalToWorld, Matrix4x4f32::identity());

  cmdList.draw(0, 4);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
