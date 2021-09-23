#include <basalt/sandbox/tribase/02-03_dreieck.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/scene_view.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/scene.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/shared/config.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <array>

using namespace std::literals;

using std::string_view;

using basalt::Debug;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::PI;
using basalt::Scene;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::DrawablePtr;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace tribase {

namespace {

auto create_camera() -> Camera {
  return Camera {Vector3f32 {},
                 Vector3f32 {0.0f, 0.0f, 1.0f},
                 Vector3f32 {0.0f, 1.0f, 0.0f},
                 PI / 2.0f,
                 0.1f,
                 100.0f};
}

} // namespace

Dreieck::Dreieck(Engine& engine)
  : mScene {std::make_shared<Scene>()}
  , mSceneView {std::make_shared<SceneView>(mScene, create_camera())}
  , mEntity {mScene->create_entity()} {
  mScene->set_background(Color::from_rgba(0, 0, 63));

  mEntity.get<Transform>().move(0.0f, 0.0f, 2.0f);

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuseA8R8G8B8_U32};

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    ColorEncoding::A8R8G8B8 diffuse;
  };

  std::array<Vertex, 3> triangleVertices {
    Vertex {0.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255u, 0, 0)},
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
  };

  MeshDescriptor meshDesc {as_bytes(gsl::span {triangleVertices}), vertexLayout,
                           PrimitiveType::TriangleList, 1};
  mTriangleMesh = engine.gfx_resource_cache().create_mesh(meshDesc);

  MaterialDescriptor materialDesc;
  materialDesc.cullBackFace = false;
  materialDesc.lit = false;

  mSolidMaterial = engine.gfx_resource_cache().create_material(materialDesc);

  materialDesc.cullBackFace = false;
  materialDesc.lit = false;
  materialDesc.solid = false;

  mWireframeMaterial =
    engine.gfx_resource_cache().create_material(materialDesc);

  std::array<Vertex, 4> quadVertices {
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
    Vertex {1.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255u, 0, 0)},
    Vertex {-1.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255u, 0, 255u)},
  };

  meshDesc = MeshDescriptor {as_bytes(gsl::span {quadVertices}), vertexLayout,
                             PrimitiveType::TriangleStrip, 2};
  mQuadMesh = engine.gfx_resource_cache().create_mesh(meshDesc);

  mEntity.emplace<RenderComponent>(mTriangleMesh, mSolidMaterial);
}

auto Dreieck::name() -> string_view {
  return "Bsp. 02-03: Das erste Dreieck"sv;
}

auto Dreieck::drawable() -> DrawablePtr {
  return mSceneView;
}

void Dreieck::tick(Engine& engine) {
  // 90° per second
  const f32 radOffsetY {PI / 2.0f * static_cast<f32>(engine.delta_time())};
  auto& transform {mEntity.get<Transform>()};
  transform.rotate(0.0f, radOffsetY, 0.0f);

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }

  if (ImGui::Begin("Settings##TribaseDreieck")) {
    if (ImGui::RadioButton("No Exercise", &mCurrentExercise, 0)) {
      mEntity.get<RenderComponent>().mesh = mTriangleMesh;
      mEntity.get<RenderComponent>().material = mSolidMaterial;
      transform.scale.set(1.0f);
    }

    if (ImGui::RadioButton("Exercise 1", &mCurrentExercise, 1)) {
      mEntity.get<RenderComponent>().mesh = mTriangleMesh;
      mEntity.get<RenderComponent>().material = mSolidMaterial;
      transform.scale.set(1.0f);
    }

    if (mCurrentExercise == 1) {
      ImGui::SameLine();
      ImGui::TextUnformatted("TODO");
    }

    if (ImGui::RadioButton("Exercise 2", &mCurrentExercise, 2)) {
      mEntity.get<RenderComponent>().mesh = mQuadMesh;
      mEntity.get<RenderComponent>().material = mSolidMaterial;
      transform.scale.set(1.0f);
    }

    if (ImGui::RadioButton("Exercise 3", &mCurrentExercise, 3)) {
      mEntity.get<RenderComponent>().mesh = mTriangleMesh;
      mEntity.get<RenderComponent>().material = mWireframeMaterial;
      transform.scale.set(1.0f);
    }

    if (ImGui::RadioButton("Exercise 4", &mCurrentExercise, 4)) {
      mEntity.get<RenderComponent>().mesh = mTriangleMesh;
      mEntity.get<RenderComponent>().material = mSolidMaterial;
      mTimeAccum = 0.0;
    }

    if (mCurrentExercise == 4) {
      mTimeAccum += engine.delta_time();

      const f32 scale {
        1.0f +
        (std::sin(PI / 2.0f * static_cast<f32>(mTimeAccum)) + 1.0f) / 2.0f};

      transform.scale.x() = scale;
      transform.scale.y() = scale;
    }
  }

  ImGui::End();
}

auto Dreieck::do_handle_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::No;
}

} // namespace tribase
