#include <basalt/sandbox/tribase/02-04_textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/asserts.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <array>
#include <random>
#include <string_view>
#include <utility>

using namespace std::literals;
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
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureAddressMode;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace tribase {

namespace {

struct Vertex final {
  Vector3f32 pos{};
  ColorEncoding::A8R8G8B8 color{};
  Vector2f32 uv{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
    VertexElement::TextureCoords2F32,
  };
};

using Distribution = uniform_real_distribution<float>;

} // namespace

Textures::Textures(Engine& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mSamplerPoint{mGfxCache->create_sampler({})}
  , mSamplerLinearWithMip{[&] {
    auto desc = SamplerDescriptor{};
    desc.magFilter = TextureFilter::Bilinear;
    desc.minFilter = TextureFilter::Bilinear;
    desc.mipFilter = TextureMipFilter::Linear;

    return mGfxCache->create_sampler(desc);
  }()}
  , mSamplerAnisotropic{[&] {
    auto desc = SamplerDescriptor{};
    desc.magFilter = TextureFilter::Bilinear;
    desc.minFilter = TextureFilter::Anisotropic;
    desc.mipFilter = TextureMipFilter::None;

    auto const& gfxInfo = engine.gfx_info();
    BASALT_ASSERT(gfxInfo.currentDeviceCaps.samplerMinFilterAnisotropic);
    desc.maxAnisotropy = gfxInfo.currentDeviceCaps.samplerMaxAnisotropy;

    return mGfxCache->create_sampler(desc);
  }()}
  , mTexture{mGfxCache->load_texture("data/tribase/Texture.bmp"sv)}
  , mPipeline{[&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto desc = PipelineDescriptor{};
    desc.fragmentShader = &fs;
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    desc.depthTest = TestPassCond::IfLessEqual;
    desc.depthWriteEnable = true;
    desc.dithering = true;

    return mGfxCache->create_pipeline(desc);
  }()} {
  auto randomEngine = default_random_engine{random_device{}()};
  auto scaleRng = Distribution{1.0f, 5.0f};
  auto rng2 = Distribution{-1.0f, 1.0f};
  auto rng3 = Distribution{0.1f, 5.0f};
  auto rng4 = Distribution{0.0f, 1.0f};
  auto rng5 = Distribution{-1.0f, 2.0f};

  auto const normalizedRandomVector = [&] {
    return Vector3f32::normalize(rng2(randomEngine), rng2(randomEngine),
                                 rng2(randomEngine));
  };

  auto vertices = vector<Vertex>{uSize{3} * sNumTriangles};
  constexpr auto startPos = Vector3f32{0.0f, 0.0f, 50.0f};

  for (auto i = uSize{0}; i < sNumTriangles; ++i) {
    auto& triangle = mTriangles[i];
    triangle.position = startPos;
    triangle.scale = scaleRng(randomEngine);

    auto const direction = normalizedRandomVector();
    auto const velocity = direction * rng3(randomEngine);
    triangle.velocity = velocity;

    auto const rotationVelocity =
      Vector3f32{rng2(randomEngine), rng2(randomEngine), rng2(randomEngine)};
    triangle.rotationVelocity = rotationVelocity;

    for (auto j = uSize{0}; j < 3; ++j) {
      vertices[3 * i + j] = {
        normalizedRandomVector(),
        Color::from_non_linear(rng4(randomEngine), rng4(randomEngine),
                               rng4(randomEngine))
          .to_argb(),
        {rng5(randomEngine), rng5(randomEngine)},
      };
    }
  }

  auto const vertexData = as_bytes(span{vertices});
  mVertexBuffer = mGfxCache->create_vertex_buffer(
    {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
}

auto Textures::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;

  auto const dt = ctx.deltaTime.count();
  auto const t = mTime.count();
  auto const currentSamplingMode{static_cast<i32>(t / 3.0) % 3};

  if (ImGui::Begin("Textures##TribaseTextures")) {
    ImGui::TextUnformatted("Hold SPACE to stop animating");
    ImGui::TextUnformatted([&] {
      if (currentSamplingMode == 0) {
        return "MIN: Linear, MAG: Linear, MIP: Linear";
      }
      if (currentSamplingMode == 1) {
        return "MIN: Point, MAG: Point, MIP: None";
      }

      return "MIN: Anisotropic, MAG: Linear, MIP: None";
    }());
  }

  ImGui::End();

  if (!is_key_down(Key::Space)) {
    for (auto i = uSize{0}; i < sNumTriangles; ++i) {
      auto& triangle = mTriangles[i];

      triangle.position += triangle.velocity * dt;
      triangle.rotation += triangle.rotationVelocity * dt;

      if (triangle.position.length() > 100.0f) {
        triangle.velocity *= -1.0f;
      }
    }
  }

  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 63), 1.0f);
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, [&] {
    if (currentSamplingMode == 0) {
      return mSamplerLinearWithMip;
    }
    if (currentSamplingMode == 1) {
      return mSamplerPoint;
    }

    return mSamplerAnisotropic;
  }());
  cmdList.bind_texture(0, mTexture);

  auto const& drawCtx = ctx.drawCtx;
  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(90_deg, aspectRatio, 0.1f, 100.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  for (auto i = u32{0}; i < sNumTriangles; ++i) {
    auto const& triangle = mTriangles[i];
    
    cmdList.set_transform(
      TransformState::LocalToWorld,
      Matrix4x4f32::scaling(triangle.scale) *
        Matrix4x4f32::rotation_x(Angle::radians(triangle.rotation.x())) *
        Matrix4x4f32::rotation_y(Angle::radians(triangle.rotation.y())) *
        Matrix4x4f32::rotation_z(Angle::radians(triangle.rotation.z())) *
        Matrix4x4f32::translation(triangle.position));
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(3 * i, 3);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto Textures::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
