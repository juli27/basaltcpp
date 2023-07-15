#include <basalt/sandbox/d3d9_tutorials.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui/imgui.h>

#include <array>
#include <memory>
#include <string_view>
#include <utility>

using namespace std::literals;

using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::Vector3f32;
using basalt::View;
using basalt::ViewPtr;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::DirectionalLight;
using basalt::gfx::Light;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCachePtr;
using basalt::gfx::Sampler;
using basalt::gfx::TestPassCond;
using basalt::gfx::Texture;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureCoordinateSource;
using basalt::gfx::TextureTransformMode;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBuffer;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;
using basalt::gfx::ext::XModel;

namespace {

struct Vertex1 final {
  array<f32, 4> pos {};
  ColorEncoding::A8R8G8B8 color {};

  static constexpr array sLayout {VertexElement::PositionTransformed4F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

struct Vertex2 final {
  array<f32, 3> pos {};
  ColorEncoding::A8R8G8B8 color {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

struct Vertex3 final {
  array<f32, 3> pos {};
  array<f32, 3> normal {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::Normal3F32};
};

struct Vertex4 final {
  array<f32, 3> pos {};
  ColorEncoding::A8R8G8B8 color {};
  array<f32, 2> uv {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8,
                                  VertexElement::TextureCoords2F32};
};

[[nodiscard]] auto create_default_world_to_view_transform() noexcept
  -> Matrix4x4f32 {
  constexpr Vector3f32 pos {0.0f, 3.0f, -5.0f};
  constexpr Vector3f32 lookAt {0.0f};

  return Matrix4x4f32::look_at_lh(pos, lookAt, Vector3f32::up());
}

[[nodiscard]] auto
create_default_view_to_clip_transform(const basalt::Size2Du16 viewport) noexcept
  -> Matrix4x4f32 {
  constexpr auto fov {45_deg};
  const f32 aspectRatio {viewport.aspect_ratio()};

  return Matrix4x4f32::perspective_projection(fov, aspectRatio, 1.0f, 100.0f);
}

class Device final : public View {
public:
  Device() noexcept = default;

protected:
  auto on_update(UpdateContext& ctx) -> void override {
    CommandList cmdList;
    cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                              Colors::BLUE);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

class Vertices final : public View {
public:
  explicit Vertices(Engine& engine)
    : mGfxCache {engine.create_gfx_resource_cache()} {
    const array vertices {
      Vertex1 {{150.0f, 50.0f, 0.5f, 1.0f}, 0xffff0000_a8r8g8b8},
      Vertex1 {{250.0f, 250.0f, 0.5f, 1.0f}, 0xff00ff00_a8r8g8b8},
      Vertex1 {{50.0f, 250.0f, 0.5f, 1.0f}, 0xff00ffff_a8r8g8b8}};
    using Vertex = decltype(vertices)::value_type;

    const auto vertexData {as_bytes(span {vertices})};
    mVertexBuffer = mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

    PipelineDescriptor pipelineDesc;
    pipelineDesc.vertexInputState = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    mPipeline = mGfxCache->create_pipeline(pipelineDesc);
  }

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() noexcept override {
    mGfxCache->destroy(mPipeline);
    mGfxCache->destroy(mVertexBuffer);
  }

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

protected:
  auto on_update(UpdateContext& ctx) -> void override {
    CommandList cmdList;
    cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                              Colors::BLUE);
    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, 3);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }

private:
  ResourceCachePtr mGfxCache;
  VertexBuffer mVertexBuffer;
  Pipeline mPipeline;
};

class Matrices final : public View {
public:
  explicit Matrices(Engine& engine)
    : mGfxCache {engine.create_gfx_resource_cache()} {
    const array vertices {
      Vertex2 {{-1.0f, -1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
      Vertex2 {{1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
      Vertex2 {{0.0f, 1.0f, 0.0f}, 0xffffffff_a8r8g8b8},
    };
    using Vertex = decltype(vertices)::value_type;

    const auto vertexData {as_bytes(span {vertices})};
    mVertexBuffer = mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

    PipelineDescriptor pipelineDesc;
    pipelineDesc.vertexInputState = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    mPipeline = mGfxCache->create_pipeline(pipelineDesc);
  }

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() noexcept override {
    mGfxCache->destroy(mPipeline);
    mGfxCache->destroy(mVertexBuffer);
  }

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

protected:
  auto on_update(UpdateContext& ctx) -> void override {
    const f32 dt {ctx.deltaTime.count()};

    constexpr f32 twoPi {PI * 2.0f};
    // 1 full rotation per second
    mRotationY += Angle::radians(twoPi * dt);
    while (mRotationY.radians() > PI) {
      mRotationY -= Angle::radians(twoPi);
    }

    CommandList cmdList;
    cmdList.clear_attachments(Attachments {Attachment::RenderTarget});
    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer);

    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_y(mRotationY));

    const Matrix4x4f32 worldToView {create_default_world_to_view_transform()};
    cmdList.set_transform(TransformState::WorldToView, worldToView);

    const Matrix4x4f32 viewToClip {
      create_default_view_to_clip_transform(ctx.drawCtx.viewport)};
    cmdList.set_transform(TransformState::ViewToClip, viewToClip);

    cmdList.draw(0, 3);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }

private:
  ResourceCachePtr mGfxCache;
  VertexBuffer mVertexBuffer;
  Pipeline mPipeline;
  Angle mRotationY;
};

class Lights final : public View {
  static constexpr u32 sVertexCount {2 * 50};

public:
  explicit Lights(Engine& engine)
    : mGfxCache {engine.create_gfx_resource_cache()} {
    array<Vertex3, sVertexCount> vertices {};
    for (uSize i {0}; i < 50; i++) {
      const Angle theta {
        Angle::radians(2.0f * PI * static_cast<f32>(i) / (50 - 1))};
      const f32 sinTheta {theta.sin()};
      const f32 cosTheta {theta.cos()};

      auto& vertex1 {vertices[2 * i]};
      vertex1.pos = {sinTheta, -1.0f, cosTheta};
      vertex1.normal = {sinTheta, 0.0f, cosTheta};

      auto& vertex2 {vertices[2 * i + 1]};
      vertex2.pos = {sinTheta, 1.0f, cosTheta};
      vertex2.normal = {sinTheta, 0.0f, cosTheta};
    }
    using Vertex = decltype(vertices)::value_type;

    const auto vertexData {as_bytes(span {vertices})};
    mVertexBuffer = mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

    PipelineDescriptor pipelineDesc;
    pipelineDesc.vertexInputState = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.lightingEnabled = true;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;
    mPipeline = mGfxCache->create_pipeline(pipelineDesc);
  }

  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;

  ~Lights() noexcept override {
    mGfxCache->destroy(mPipeline);
    mGfxCache->destroy(mVertexBuffer);
  }

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights&&) -> Lights& = delete;

private:
  ResourceCachePtr mGfxCache;
  VertexBuffer mVertexBuffer;
  Pipeline mPipeline;
  Angle mRotationX;
  Angle mLightRotation;

  auto on_update(UpdateContext& ctx) -> void override {
    const f32 dt {ctx.deltaTime.count()};

    constexpr f32 twoPi {2.0f * PI};

    mRotationX += Angle::radians(2.0f * dt);
    while (mRotationX.radians() > PI) {
      mRotationX -= Angle::radians(twoPi);
    }

    mLightRotation += Angle::radians(20.0f / 7.0f * dt);
    while (mLightRotation.radians() > PI) {
      mLightRotation -= Angle::radians(twoPi);
    }

    CommandList cmdList;
    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f);

    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer);

    const auto viewToClip {
      create_default_view_to_clip_transform(ctx.drawCtx.viewport)};
    cmdList.set_transform(TransformState::ViewToClip, viewToClip);

    const auto worldToView {create_default_world_to_view_transform()};
    cmdList.set_transform(TransformState::WorldToView, worldToView);

    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    DirectionalLight directionalLight;
    directionalLight.diffuse = Color::from_non_linear(1.0f, 1.0f, 1.0f, 0.0f);
    directionalLight.directionInWorld =
      Vector3f32::normalize(mLightRotation.cos(), 1.0f, mLightRotation.sin());
    const Light light {directionalLight};

    cmdList.set_lights(span<const Light> {&light, 1});
    cmdList.set_ambient_light(Color::from(0x00202020_a8r8g8b8));
    cmdList.set_material(Colors::YELLOW, Colors::YELLOW);

    cmdList.draw(0, sVertexCount);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

class Textures final : public View {
  static constexpr u32 sVertexCount {2 * 50};

public:
  explicit Textures(Engine& engine)
    : mGfxCache {engine.create_gfx_resource_cache()}
    , mSampler {mGfxCache->create_sampler({})}
    , mTexture {mGfxCache->load_texture("data/banana.bmp")} {
    array<Vertex4, sVertexCount> vertices {};
    for (uSize i {0}; i < 50; i++) {
      const Angle theta {
        Angle::radians(2.0f * PI * static_cast<f32>(i) / (50 - 1))};
      const f32 sinTheta {theta.sin()};
      const f32 cosTheta {theta.cos()};

      Vertex4& vertex1 {vertices[2 * i]};
      vertex1.pos = {sinTheta, -1.0f, cosTheta};
      vertex1.color = Colors::WHITE.to_argb();
      vertex1.uv = {
        static_cast<f32>(i) / (50.0f - 1.0f),
        1.0f,
      };

      Vertex4& vertex2 {vertices[2 * i + 1]};
      vertex2.pos = {sinTheta, 1.0f, cosTheta};
      vertex2.color = 0xff808080_a8r8g8b8;
      vertex2.uv = {
        std::get<0>(vertex1.uv),
        0.0f,
      };
    }
    using Vertex = decltype(vertices)::value_type;

    const auto vertexData {as_bytes(span {vertices})};
    mVertexBuffer = mGfxCache->create_vertex_buffer(
      {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

    TextureBlendingStage textureBlendingStage;
    PipelineDescriptor pipelineDesc;
    pipelineDesc.vertexInputState = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.textureStages = span {&textureBlendingStage, 1};
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;
    mPipeline = mGfxCache->create_pipeline(pipelineDesc);

    textureBlendingStage.texCoordinateSrc =
      TextureCoordinateSource::VertexPositionInView;
    textureBlendingStage.texCoordinateTransformMode =
      TextureTransformMode::Count4;
    textureBlendingStage.texCoordinateProjected = true;
    mPipelineTci = mGfxCache->create_pipeline(pipelineDesc);
  }

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() noexcept override {
    mGfxCache->destroy(mPipelineTci);
    mGfxCache->destroy(mPipeline);
    mGfxCache->destroy(mTexture);
    mGfxCache->destroy(mSampler);
    mGfxCache->destroy(mVertexBuffer);
  }

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

private:
  ResourceCachePtr mGfxCache;
  VertexBuffer mVertexBuffer;
  Sampler mSampler;
  Texture mTexture;
  Pipeline mPipeline;
  Pipeline mPipelineTci;
  Angle mRotationX;
  bool mShowTci {};

  auto on_update(UpdateContext& ctx) -> void override {
    const f32 dt {ctx.deltaTime.count()};

    mRotationX += Angle::radians(dt);
    while (mRotationX.radians() > PI) {
      mRotationX -= Angle::radians(PI * 2.0f);
    }

    if (ImGui::Begin("Settings##D3D9Textures")) {
      ImGui::Checkbox("Show TCI", &mShowTci);
    }

    ImGui::End();

    CommandList cmdList;
    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f);

    cmdList.bind_pipeline(mShowTci ? mPipelineTci : mPipeline);
    cmdList.bind_sampler(mSampler);
    cmdList.bind_texture(mTexture);
    cmdList.bind_vertex_buffer(mVertexBuffer);

    const auto viewToClip {
      create_default_view_to_clip_transform(ctx.drawCtx.viewport)};
    cmdList.set_transform(TransformState::ViewToClip, viewToClip);

    const auto worldToView {create_default_world_to_view_transform()};
    cmdList.set_transform(TransformState::WorldToView, worldToView);

    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    if (mShowTci) {
      const auto texTransform {viewToClip *
                               Matrix4x4f32::scaling(0.5f, -0.5f, 1.0f) *
                               Matrix4x4f32::translation(0.5f, 0.5f, 0.0f)};
      cmdList.set_transform(TransformState::Texture, texTransform);
    }

    cmdList.draw(0, sVertexCount);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

class Meshes final : public View {
public:
  explicit Meshes(Engine& engine)
    : mGfxCache {engine.create_gfx_resource_cache()}
    , mModel {mGfxCache->load_x_model("data/Tiger.x"sv)} {
  }

  Meshes(const Meshes&) = delete;
  Meshes(const Meshes&&) = delete;

  ~Meshes() noexcept override {
    mGfxCache->destroy(mModel);
  }

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(const Meshes&&) -> Meshes& = delete;

private:
  ResourceCachePtr mGfxCache;
  XModel mModel;
  Angle mRotationY;

  auto on_update(UpdateContext& ctx) -> void override {
    const f32 dt {ctx.deltaTime.count()};

    mRotationY += Angle::radians(dt);
    while (mRotationY.radians() > PI) {
      mRotationY -= Angle::radians(PI * 2.0f);
    }

    CommandList cmdList;
    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f);

    cmdList.set_ambient_light(Colors::WHITE);

    const auto viewToClip {
      create_default_view_to_clip_transform(ctx.drawCtx.viewport)};
    cmdList.set_transform(TransformState::ViewToClip, viewToClip);

    const auto worldToView {create_default_world_to_view_transform()};
    cmdList.set_transform(TransformState::WorldToView, worldToView);

    const auto& modelData {mGfxCache->get(mModel)};

    const u32 numMaterials {static_cast<u32>(modelData.materials.size())};
    for (u32 i {0}; i < numMaterials; ++i) {
      const auto& materialData {mGfxCache->get(modelData.materials[i])};

      cmdList.bind_pipeline(materialData.pipeline);

      cmdList.bind_sampler(materialData.sampler);
      cmdList.bind_texture(materialData.texture);
      cmdList.set_material(materialData.diffuse, materialData.ambient);

      cmdList.set_transform(TransformState::LocalToWorld,
                            Matrix4x4f32::rotation_y(mRotationY));

      XMeshCommandEncoder::draw_x_mesh(cmdList, modelData.mesh, i);
    }

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

} // namespace

auto D3D9Tutorials::new_vertices_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Vertices>(engine);
}

auto D3D9Tutorials::new_matrices_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Matrices>(engine);
}

auto D3D9Tutorials::new_lights_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Lights>(engine);
}

auto D3D9Tutorials::new_textures_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Textures>(engine);
}

auto D3D9Tutorials::new_meshes_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Meshes>(engine);
}

auto D3D9Tutorials::new_device_tutorial(Engine&) -> ViewPtr {
  return std::make_shared<Device>();
}
