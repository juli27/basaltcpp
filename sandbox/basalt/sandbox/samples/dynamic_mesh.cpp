#include <basalt/api/view.h>

#include <basalt/sandbox/samples/samples.h>

#include <basalt/sandbox/shared/debug_scene_view.h>
#include <basalt/sandbox/shared/rotation_system.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/mesh.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>
#include <utility>

using namespace basalt;
using namespace std::literals;

namespace {

struct Vertex {
  Vector3f32 position;
  ColorEncoding::A8R8G8B8 color;

  static constexpr auto sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8>();
};

constexpr auto QUAD_VERTICES = std::array{
  Vertex{{1.0f, -1.0f, 0.0f}, Colors::RED.to_argb()},
  Vertex{{-1.0f, -1.0f, 0.0f}, Colors::GREEN.to_argb()},
  Vertex{{1.0f, 1.0f, 0.0f}, Colors::BLUE.to_argb()},
  Vertex{{-1.0f, 1.0f, 0.0f}, 0xff00ffff_a8r8g8b8},
};

class DynamicMeshView final : public View {
public:
  DynamicMeshView(SceneViewPtr sceneView, EntityId const quadId,
                  gfx::MaterialHandle const solidMaterial,
                  gfx::MaterialHandle const wireframeMaterial,
                  gfx::MeshHandle const mesh)
    : mScene{sceneView->scene()}
    , mQuadId{quadId}
    , mSolidMaterial{solidMaterial}
    , mWireframeMaterial{wireframeMaterial}
    , mMesh{mesh} {
    add_child_bottom(std::move(sceneView));
  }

private:
  ScenePtr mScene;
  EntityId mQuadId;
  gfx::MaterialHandle mSolidMaterial;
  gfx::MaterialHandle mWireframeMaterial;
  gfx::MeshHandle mMesh;
  SecondsF32 mColorTime{0};
  SecondsF32 mScaleTime{0};
  bool mAnimateColors{false};
  bool mAnimateScale{false};

  auto on_update(UpdateContext& ctx) -> void override {
    auto const quad = mScene->get_handle(mQuadId);

    if (ImGui::Begin("Settings##DynamicMesh")) {
      auto const isRenderWireframe =
        quad.get<gfx::Model const>().material == mWireframeMaterial;
      if (ImGui::RadioButton("Render solid", !isRenderWireframe)) {
        quad.patch<gfx::Model>(
          [&](gfx::Model& model) { model.material = mSolidMaterial; });
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Render wireframe", isRenderWireframe)) {
        quad.patch<gfx::Model>(
          [&](gfx::Model& model) { model.material = mWireframeMaterial; });
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

    auto& gfxCtx = ctx.engine.gfx_context();
    auto const& mesh = gfxCtx.get(mMesh);

    gfxCtx.with_mapping_of(
      mesh.vertexBuffer(), [&](gsl::span<std::byte> const data) {
        auto const vertexData = as_bytes(gsl::span{vertices});
        std::copy_n(vertexData.begin(),
                    std::min(vertexData.size_bytes(), data.size_bytes()),
                    data.begin());
      });

    if (mAnimateScale) {
      mScaleTime += ctx.deltaTime;
    }

    quad.patch<Transform>([&](Transform& transform) {
      auto const scaleT = mScaleTime.count();
      transform.scale = Vector3f32{1.0f + std::sin(PI / 2.0f * scaleT) / 2.0f};
    });
  }
};

} // namespace

auto Samples::new_dymanic_mesh_sample(Engine& engine) -> ViewPtr {
  auto scene = Scene::create();
  scene->create_system<RotationSystem>();
  auto& ecsCtx = scene->entity_registry().ctx();
  auto& gfxEnv = ecsCtx.emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(0, 0, 63));

  auto gfxCache = engine.create_gfx_resource_cache();

  auto const solidMaterial = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;
      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;

      return gfxCache->create_material_class(info);
    }();

    return gfxCache->create_material(info);
  }();

  auto const wireframeMaterial = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;
      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;
      pipelineInfo.fillMode = gfx::FillMode::Wireframe;

      return gfxCache->create_material_class(info);
    }();

    return gfxCache->create_material(info);
  }();

  auto const quadMesh = [&] {
    auto const vertexData = as_bytes(gsl::span{QUAD_VERTICES});

    auto info = gfx::MeshCreateInfo{};
    info.vertexBuffer = gfxCache->create_vertex_buffer(
      gfx::VertexBufferCreateInfo{vertexData.size_bytes(), Vertex::sLayout},
      vertexData);
    info.vertexCount = static_cast<u32>(QUAD_VERTICES.size());

    return gfxCache->create_mesh(info);
  }();

  auto const quad = scene->create_entity("Quad"s, Vector3f32{0.0f, 0.0f, 2.0f});
  quad.emplace<gfx::Model>(quadMesh, solidMaterial);
  quad.emplace<RotationSpeed>(0.0f, PI / 2.0f, 0.0f);

  auto const camera = scene->create_entity("Camera"s);
  camera.emplace<gfx::Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                              0.1f, 100.0f);
  auto sceneView = DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                          camera.entity());

  return std::make_shared<DynamicMeshView>(std::move(sceneView), quad.entity(),
                                           solidMaterial, wireframeMaterial,
                                           quadMesh);
}
