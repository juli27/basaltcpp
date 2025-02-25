#include <basalt/sandbox/d3d9_tutorials.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/vertex_layout.h>
#include <basalt/api/gfx/backend/ext/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui.h>

#include <array>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;
using std::array;
using std::vector;

using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::View;
using basalt::ViewPtr;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::DirectionalLightData;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::LightData;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::MaterialHandle;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PipelineHandle;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCachePtr;
using basalt::gfx::SamplerHandle;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureCoordinateSet;
using basalt::gfx::TextureCoordinateSrc;
using basalt::gfx::TextureCoordinateTransformMode;
using basalt::gfx::TextureHandle;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferHandle;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;
using basalt::gfx::ext::XMeshHandle;

namespace {

[[nodiscard]]
auto create_world_to_view_transform() noexcept -> Matrix4x4f32 {
  return Matrix4x4f32::look_at_lh({0.0f, 3.0f, -5.0f}, {}, Vector3f32::up());
}

[[nodiscard]]
auto create_view_to_clip_transform(basalt::Size2Du16 const viewport) noexcept
  -> Matrix4x4f32 {
  return Matrix4x4f32::perspective_projection(45_deg, viewport.aspect_ratio(),
                                              1.0f, 100.0f);
}

class Matrices final : public View {
  struct Vertex {
    Vector3f32 pos{};
    ColorEncoding::A8R8G8B8 color{};

    static constexpr auto sLayout = basalt::gfx::make_vertex_layout<
      VertexElement::Position3F32, VertexElement::ColorDiffuse1U32A8R8G8B8>();
  };

public:
  explicit Matrices(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mVertexBuffer{[&] {
      constexpr auto vertices = array{
        Vertex{{-1.0f, -1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
        Vertex{{1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
        Vertex{{0.0f, 1.0f, 0.0f}, 0xffffffff_a8r8g8b8},
      };

      auto const vertexData = as_bytes(span{vertices});
      return mGfxCache->create_vertex_buffer(
        {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
    }()}
    , mPipeline{[&] {
      auto desc = PipelineCreateInfo{};
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = PrimitiveType::TriangleStrip;
      return mGfxCache->create_pipeline(desc);
    }()} {
  }

protected:
  auto on_update(UpdateContext& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    // per second
    constexpr auto rotationSpeedRadians = 2.0f * PI;
    mRotationY += Angle::radians(rotationSpeedRadians * dt);

    auto cmdList = CommandList{};
    cmdList.clear_attachments(Attachments{Attachment::RenderTarget});
    cmdList.bind_pipeline(mPipeline);
    cmdList.set_transform(TransformState::ViewToClip,
                          create_view_to_clip_transform(ctx.drawCtx.viewport));
    cmdList.set_transform(TransformState::WorldToView,
                          create_world_to_view_transform());
    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_y(mRotationY));
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, 3);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }

private:
  ResourceCachePtr mGfxCache;
  VertexBufferHandle mVertexBuffer;
  PipelineHandle mPipeline;
  Angle mRotationY;
};

class Lights final : public View {
  static constexpr u32 sVertexCount{2 * 50};

  struct Vertex {
    Vector3f32 pos{};
    Vector3f32 normal{};

    static constexpr auto sLayout =
      basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                      VertexElement::Normal3F32>();
  };

public:
  explicit Lights(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mVertexBuffer{[&] {
      auto vertices = array<Vertex, sVertexCount>{};
      for (auto i = uSize{0}; i < 50; i++) {
        auto const theta =
          Angle::radians(2.0f * PI * static_cast<f32>(i) / (50.0f - 1.0f));
        auto const sinTheta = theta.sin();
        auto const cosTheta = theta.cos();

        auto const normal = Vector3f32{sinTheta, 0.0f, cosTheta};
        vertices[2 * i] = {{sinTheta, -1.0f, cosTheta}, normal};
        vertices[2 * i + 1] = {{sinTheta, 1.0f, cosTheta}, normal};
      }

      auto const vertexData = as_bytes(span{vertices});
      return mGfxCache->create_vertex_buffer(
        {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
    }()}
    , mPipeline{[&] {
      auto vs = FixedVertexShaderCreateInfo{};
      vs.lightingEnabled = true;

      auto desc = PipelineCreateInfo{};
      desc.vertexShader = &vs;
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = PrimitiveType::TriangleStrip;
      desc.depthTest = TestPassCond::IfLessEqual;
      desc.depthWriteEnable = true;
      return mGfxCache->create_pipeline(desc);
    }()} {
  }

private:
  ResourceCachePtr mGfxCache;
  VertexBufferHandle mVertexBuffer;
  PipelineHandle mPipeline;
  Angle mRotationX;
  Angle mLightRotation;

  auto on_update(UpdateContext& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    mRotationX += Angle::radians(2.0f * dt);
    mLightRotation += Angle::radians(20.0f / 7.0f * dt);

    auto cmdList = CommandList{};
    cmdList.clear_attachments(
      Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f);

    cmdList.bind_pipeline(mPipeline);

    cmdList.set_transform(TransformState::ViewToClip,
                          create_view_to_clip_transform(ctx.drawCtx.viewport));
    cmdList.set_transform(TransformState::WorldToView,
                          create_world_to_view_transform());
    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    auto directionalLight = DirectionalLightData{};
    directionalLight.diffuse = Color::from_non_linear(1.0f, 1.0f, 1.0f, 0.0f);
    directionalLight.directionInWorld =
      Vector3f32::normalize(mLightRotation.cos(), 1.0f, mLightRotation.sin());
    auto const lights = array{LightData{directionalLight}};
    cmdList.set_lights(lights);
    cmdList.set_ambient_light(Color::from(0x00202020_a8r8g8b8));
    cmdList.set_material(Colors::YELLOW, Colors::YELLOW);
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, sVertexCount);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

class Textures final : public View {
  static constexpr auto sVertexCount = u32{2 * 50};
  static constexpr auto sTextureFilePath = "data/banana.bmp"sv;

  struct Vertex {
    Vector3f32 pos{};
    ColorEncoding::A8R8G8B8 color{};
    Vector2f32 uv{};

    static constexpr auto sLayout =
      basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                      VertexElement::ColorDiffuse1U32A8R8G8B8,
                                      VertexElement::TextureCoords2F32>();
  };

public:
  explicit Textures(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()}
    , mSampler{mGfxCache->create_sampler({})}
    , mTexture{mGfxCache->load_texture_2d(sTextureFilePath)}
    , mVertexBuffer{[&] {
      auto vertices = array<Vertex, sVertexCount>{};
      for (auto i = uSize{0}; i < 50; i++) {
        auto const theta =
          Angle::radians(2.0f * PI * static_cast<f32>(i) / (50.0f - 1.0f));
        auto const sinTheta = theta.sin();
        auto const cosTheta = theta.cos();
        auto const u = static_cast<f32>(i) / (50.0f - 1.0f);

        vertices[2 * i] = {
          {sinTheta, -1, cosTheta},
          Colors::WHITE.to_argb(),
          {u, 1},
        };
        vertices[2 * i + 1] = {
          {sinTheta, 1, cosTheta},
          0xff808080_a8r8g8b8,
          {u, 0},
        };
      }

      auto const vertexData = as_bytes(span{vertices});
      return mGfxCache->create_vertex_buffer(
        {vertexData.size_bytes(), Vertex::sLayout}, vertexData);
    }()}
    , mPipeline{[&] {
      auto fs = FixedFragmentShaderCreateInfo{};
      constexpr auto textureStages = array{TextureStage{}};
      fs.textureStages = textureStages;

      auto desc = PipelineCreateInfo{};
      desc.fragmentShader = &fs;
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = PrimitiveType::TriangleStrip;
      desc.depthTest = TestPassCond::IfLessEqual;
      desc.depthWriteEnable = true;
      return mGfxCache->create_pipeline(desc);
    }()}
    , mPipelineTci{[&] {
      auto vs = FixedVertexShaderCreateInfo{};
      auto texCoordinateSets = array{TextureCoordinateSet{}};
      auto& coordinateSet = std::get<0>(texCoordinateSets);
      coordinateSet.src = TextureCoordinateSrc::PositionInViewSpace;
      coordinateSet.transformMode = TextureCoordinateTransformMode::Count4;
      coordinateSet.projected = true;
      vs.textureCoordinateSets = texCoordinateSets;

      auto fs = FixedFragmentShaderCreateInfo{};
      constexpr auto textureStages = array{TextureStage{}};
      fs.textureStages = textureStages;

      auto desc = PipelineCreateInfo{};
      desc.vertexShader = &vs;
      desc.fragmentShader = &fs;
      desc.vertexLayout = Vertex::sLayout;
      desc.primitiveType = PrimitiveType::TriangleStrip;
      desc.depthTest = TestPassCond::IfLessEqual;
      desc.depthWriteEnable = true;
      return mGfxCache->create_pipeline(desc);
    }()} {
  }

private:
  ResourceCachePtr mGfxCache;
  SamplerHandle mSampler;
  TextureHandle mTexture;
  VertexBufferHandle mVertexBuffer;
  PipelineHandle mPipeline;
  PipelineHandle mPipelineTci;
  Angle mRotationX;
  bool mShowTci{};

  auto on_update(UpdateContext& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    mRotationX += Angle::radians(dt);

    if (ImGui::Begin("Settings##D3D9Textures")) {
      ImGui::Checkbox("Show TCI", &mShowTci);
    }

    ImGui::End();

    auto cmdList = CommandList{};
    cmdList.clear_attachments(
      Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f);

    cmdList.bind_pipeline(mShowTci ? mPipelineTci : mPipeline);
    cmdList.bind_sampler(0, mSampler);
    cmdList.bind_texture(0, mTexture);

    auto const viewToClip = create_view_to_clip_transform(ctx.drawCtx.viewport);
    cmdList.set_transform(TransformState::ViewToClip, viewToClip);
    cmdList.set_transform(TransformState::WorldToView,
                          create_world_to_view_transform());
    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    if (mShowTci) {
      cmdList.set_transform(TransformState::Texture0,
                            viewToClip *
                              Matrix4x4f32::scaling(0.5f, -0.5f, 1.0f) *
                              Matrix4x4f32::translation(0.5f, 0.5f, 0.0f));
    }

    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.draw(0, sVertexCount);

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

class Meshes final : public View {
  static constexpr auto sModelFilePath = "data/Tiger.x"sv;

public:
  explicit Meshes(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()} {
    auto modelData = mGfxCache->load_x_meshes(sModelFilePath);
    mMaterials = [&] {
      auto materials = vector<MaterialHandle>{};

      auto const pipeline = [&] {
        auto vs = FixedVertexShaderCreateInfo{};
        vs.lightingEnabled = true;

        auto fs = FixedFragmentShaderCreateInfo{};
        auto textureStages = array{TextureStage{}};
        fs.textureStages = textureStages;

        auto pipelineDesc = PipelineCreateInfo{};
        pipelineDesc.vertexShader = &vs;
        pipelineDesc.fragmentShader = &fs;
        pipelineDesc.cullMode = CullMode::CounterClockwise;
        pipelineDesc.depthTest = TestPassCond::IfLessEqual;
        pipelineDesc.depthWriteEnable = true;

        return mGfxCache->create_pipeline(pipelineDesc);
      }();

      for (auto i = 0; i < modelData.materials.size(); i++) {
        auto const& material = modelData.materials[i];
        auto materialDesc = MaterialCreateInfo{};
        materialDesc.pipeline = pipeline;
        materialDesc.diffuse = material.diffuse;
        materialDesc.ambient = material.ambient;

        if (!material.textureFile.empty()) {
          materialDesc.sampledTexture.texture =
            mGfxCache->load_texture_2d(material.textureFile);
          materialDesc.sampledTexture.sampler = mGfxCache->create_sampler({});
        }

        materials.push_back(mGfxCache->create_material(materialDesc));
      }
      
      return materials;
    }();
    mMeshes = std::move(modelData.meshes);
  }

private:
  ResourceCachePtr mGfxCache;
  vector<MaterialHandle> mMaterials;
  vector<XMeshHandle> mMeshes;
  Angle mRotationY;

  auto on_update(UpdateContext& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    mRotationY += Angle::radians(dt);

    auto const& gfxCtx = ctx.engine.gfx_context();

    auto cmdList = CommandList{};
    cmdList.clear_attachments(
      Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1);
    cmdList.set_ambient_light(Colors::WHITE);
    cmdList.set_lights({});
    cmdList.set_transform(TransformState::ViewToClip,
                          create_view_to_clip_transform(ctx.drawCtx.viewport));
    cmdList.set_transform(TransformState::WorldToView,
                          create_world_to_view_transform());

    for (auto i = uSize{0}; i < mMaterials.size(); ++i) {
      auto const& materialData = gfxCtx.get(mMaterials[i]);

      cmdList.bind_pipeline(materialData.pipeline);
      cmdList.bind_sampler(0, materialData.sampler);
      cmdList.bind_texture(0, materialData.texture);
      cmdList.set_material(materialData.diffuse, materialData.ambient);
      cmdList.set_transform(TransformState::LocalToWorld,
                            Matrix4x4f32::rotation_y(mRotationY));
      XMeshCommandEncoder::draw_x_mesh(cmdList, mMeshes[i]);
    }

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

} // namespace

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
