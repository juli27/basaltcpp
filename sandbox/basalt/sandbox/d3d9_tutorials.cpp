#include <basalt/sandbox/d3d9_tutorials.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>

using namespace std::literals;

using std::array;
using std::byte;

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
using basalt::gfx::Camera;
using basalt::gfx::CommandList;
using basalt::gfx::DirectionalLight;
using basalt::gfx::Light;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCache;
using basalt::gfx::Sampler;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestOp;
using basalt::gfx::Texture;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureCoordinateSource;
using basalt::gfx::TextureTransformMode;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBuffer;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;
using basalt::gfx::ext::XModel;

namespace {

auto create_default_camera() noexcept -> Camera {
  constexpr Vector3f32 cameraPos {0.0f, 3.0f, -5.0f};
  constexpr Vector3f32 lookAt {0.0f};

  return Camera {cameraPos, lookAt, Vector3f32::up(), 45.0_deg, 1.0f, 100.0f};
}

class Device final : public View {
public:
  Device() noexcept = default;

private:
  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList {};
    cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                              Colors::BLUE, 1.0f, 0);

    context.commandLists.push_back(std::move(cmdList));
  }
};

class Vertices final : public View {
public:
  explicit Vertices(Engine& engine)
    : mResourceCache {engine.gfx_resource_cache()} {
    constexpr array<const VertexElement, 2> vertexLayout {
      VertexElement::PositionTransformed4F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
    };

    PipelineDescriptor pipelineDesc {};
    pipelineDesc.vertexInputState = vertexLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    mPipeline = mResourceCache.create_pipeline(pipelineDesc);

    struct Vertex final {
      f32 x;
      f32 y;
      f32 z;
      f32 rhw;
      ColorEncoding::A8R8G8B8 color;
    };

    const array vertices {
      Vertex {150.0f, 50.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
      Vertex {250.0f, 250.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
      Vertex {50.0f, 250.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(0, 255, 255)}};

    const auto vertexData {as_bytes(span {vertices})};

    const VertexBufferDescriptor vertexBufferDesc {
      vertexData.size_bytes(),
      VertexLayout {
        VertexElement::PositionTransformed4F32,
        VertexElement::ColorDiffuse1U32A8R8G8B8,
      },
    };
    mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() noexcept override {
    mResourceCache.destroy(mVertexBuffer);
    mResourceCache.destroy(mPipeline);
  }

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};

  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList {};

    cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                              Colors::BLUE, 1.0f, 0);

    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);
    cmdList.draw(0, 3);

    context.commandLists.push_back(std::move(cmdList));
  }
};

class Matrices final : public View {
public:
  explicit Matrices(Engine& engine)
    : mResourceCache {engine.gfx_resource_cache()}
    , mCamera {create_default_camera()} {
    constexpr array<const VertexElement, 2> vertexLayout {
      VertexElement::Position3F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
    };

    PipelineDescriptor pipelineDesc {};
    pipelineDesc.vertexInputState = vertexLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    mPipeline = mResourceCache.create_pipeline(pipelineDesc);

    struct Vertex final {
      f32 x;
      f32 y;
      f32 z;
      ColorEncoding::A8R8G8B8 color;
    };

    const array vertices {
      Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
      Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
      Vertex {0.0f, 1.0f, 0.0f,
              ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255)},
    };

    const auto vertexData {as_bytes(span {vertices})};

    const VertexBufferDescriptor vertexBufferDesc {
      vertexData.size_bytes(),
      VertexLayout {
        VertexElement::Position3F32,
        VertexElement::ColorDiffuse1U32A8R8G8B8,
      },
    };
    mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() noexcept override {
    mResourceCache.destroy(mVertexBuffer);
    mResourceCache.destroy(mPipeline);
  }

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};
  Camera mCamera;
  Angle mRotationY;

  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList;

    cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                              Colors::BLACK, 1.0f, 0);

    cmdList.bind_pipeline(mPipeline);

    cmdList.set_transform(TransformState::ModelToWorld,
                          Matrix4x4f32::rotation_y(mRotationY));
    cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());
    cmdList.set_transform(TransformState::ViewToViewport,
                          mCamera.view_to_viewport(context.viewport));

    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

    cmdList.draw(0, 3);

    context.commandLists.push_back(std::move(cmdList));
  }

  auto on_tick(Engine& engine) -> void override {
    const auto dt {static_cast<f32>(engine.delta_time())};

    constexpr f32 twoPi {PI * 2.0f};
    // 1 full rotation per second
    mRotationY += Angle::radians(twoPi * dt);
    while (mRotationY.radians() > PI) {
      mRotationY -= Angle::radians(twoPi);
    }
  }
};

class Lights final : public View {
  static constexpr u32 VERTEX_COUNT {2u * 50u};

public:
  explicit Lights(Engine& engine)
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
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;

  ~Lights() noexcept override {
    mResourceCache.destroy(mPipeline);
    mResourceCache.destroy(mVertexBuffer);
  }

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights&&) -> Lights& = delete;

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};
  Camera mCamera;
  Angle mRotationX;
  Angle mLightRotation;

  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList;

    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f, 0);

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

    cmdList.set_ambient_light(
      Color::from_non_linear_rgba8(0x20, 0x20, 0x20, 0));

    cmdList.set_material(Colors::YELLOW, Colors::YELLOW, Color {});
    cmdList.set_transform(TransformState::ModelToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

    cmdList.draw(0, VERTEX_COUNT);

    context.commandLists.push_back(std::move(cmdList));
  }

  auto on_tick(Engine& engine) -> void override {
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
};

class Textures final : public View {
  static constexpr u32 VERTEX_COUNT {2u * 50u};

public:
  explicit Textures(Engine& engine)
    : mResourceCache {engine.gfx_resource_cache()}
    , mCamera {create_default_camera()} {
    constexpr array<const VertexElement, 3> vertexLayout {
      VertexElement::Position3F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
      VertexElement::TextureCoords2F32,
    };

    TextureBlendingStage textureBlendingStage {};

    PipelineDescriptor pipelineDesc {};
    pipelineDesc.vertexInputState = vertexLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.textureStages = span {&textureBlendingStage, 1};
    pipelineDesc.depthTest = TestOp::PassIfLessEqual;
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
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const span<byte> mapping) {
        copy(vertexData.begin(), vertexData.end(), mapping.begin());
      });

    mSampler = mResourceCache.create_sampler(SamplerDescriptor {});
    mTexture = mResourceCache.load_texture("data/banana.bmp");
  }

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() noexcept override {
    mResourceCache.destroy(mTexture);
    mResourceCache.destroy(mSampler);
    mResourceCache.destroy(mVertexBuffer);
    mResourceCache.destroy(mPipelineTci);
    mResourceCache.destroy(mPipeline);
  }

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline;
  Pipeline mPipelineTci;
  VertexBuffer mVertexBuffer;
  Sampler mSampler;
  Texture mTexture;
  Camera mCamera;
  Angle mRotationX {};
  bool mShowTci {false};

  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList;

    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f, 0);

    cmdList.bind_pipeline(mShowTci ? mPipelineTci : mPipeline);

    const Matrix4x4f32 viewToViewport {
      mCamera.view_to_viewport(context.viewport)};
    cmdList.set_transform(TransformState::ViewToViewport, viewToViewport);
    cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());

    cmdList.set_transform(TransformState::ModelToWorld,
                          Matrix4x4f32::rotation_x(mRotationX));

    if (mShowTci) {
      const Matrix4x4f32 texTransform {
        viewToViewport * Matrix4x4f32::scaling(Vector3f32 {0.5f, -0.5f, 1.0f}) *
        Matrix4x4f32::translation(Vector3f32 {0.5f, 0.5f, 0.0f})};
      cmdList.set_transform(TransformState::Texture, texTransform);
    }

    cmdList.bind_sampler(mSampler);
    cmdList.bind_texture(mTexture);
    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

    cmdList.draw(0, VERTEX_COUNT);

    context.commandLists.push_back(std::move(cmdList));
  }

  auto on_tick(Engine& engine) -> void override {
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
};

class Meshes final : public View {
public:
  explicit Meshes(Engine& engine)
    : mResourceCache {engine.gfx_resource_cache()}
    , mModel {mResourceCache.load_x_model("data/Tiger.x"sv)}
    , mCamera {create_default_camera()} {
  }

  Meshes(const Meshes&) = delete;
  Meshes(const Meshes&&) = delete;

  ~Meshes() noexcept override {
    mResourceCache.destroy(mModel);
  }

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(const Meshes&&) -> Meshes& = delete;

private:
  ResourceCache& mResourceCache;
  XModel mModel;
  Camera mCamera;
  Angle mRotationY;

  auto on_draw(const DrawContext& context) -> void override {
    CommandList cmdList;

    cmdList.clear_attachments(
      Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1.0f, 0);

    cmdList.set_ambient_light(Colors::WHITE);

    cmdList.set_transform(TransformState::ViewToViewport,
                          mCamera.view_to_viewport(context.viewport));
    cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());

    const auto& modelData {mResourceCache.get(mModel)};

    const u32 numMaterials {static_cast<u32>(modelData.materials.size())};
    for (u32 i {0}; i < numMaterials; ++i) {
      const auto& materialData {mResourceCache.get(modelData.materials[i])};

      cmdList.bind_pipeline(materialData.pipeline);

      cmdList.bind_sampler(materialData.sampler);
      cmdList.bind_texture(materialData.texture);
      cmdList.set_material(materialData.diffuse, materialData.ambient,
                           Color {});

      cmdList.set_transform(TransformState::ModelToWorld,
                            Matrix4x4f32::rotation_y(mRotationY));

      cmdList.ext_draw_x_mesh(modelData.mesh, i);
    }

    context.commandLists.push_back(std::move(cmdList));
  }

  auto on_tick(Engine& engine) -> void override {
    const auto dt {static_cast<f32>(engine.delta_time())};

    mRotationY += Angle::radians(dt);
    while (mRotationY.radians() > PI) {
      mRotationY -= Angle::radians(PI * 2.0f);
    }
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
