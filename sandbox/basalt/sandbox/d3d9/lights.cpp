#include <basalt/sandbox/d3d9/lights.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::PI;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Lights::Lights(Engine& engine)
  : mSceneView {std::make_shared<SceneView>(mScene, create_default_camera())} {
  mScene->set_background(Colors::BLUE);
  mScene->set_ambient_light(Color::from_rgba(32, 32, 32));

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    f32 nx {};
    f32 ny {};
    f32 nz {};
  };

  array<Vertex, 50u * 2u> vertices {};
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

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::Normal3F32};

  const MeshDescriptor mesh {as_bytes(gsl::span(vertices)), vertexLayout,
                             PrimitiveType::TriangleStrip,
                             static_cast<u32>(vertices.size() - 2)};

  auto& rc {mCylinder.emplace<RenderComponent>()};
  rc.mesh = engine.gfx_resource_cache().create_mesh(mesh);

  MaterialDescriptor material;
  material.diffuse = Color {1.0f, 1.0f, 0.0f};
  material.ambient = Color {1.0f, 1.0f, 0.0f};
  material.cullBackFace = false;
  rc.material = engine.gfx_resource_cache().create_material(material);
}

auto Lights::name() -> string_view {
  return "Tutorial 4: Creating and Using Lights"sv;
}

auto Lights::drawable() -> basalt::gfx::DrawablePtr {
  return mSceneView;
}

void Lights::tick(Engine& engine) {
  const auto dt {static_cast<f32>(engine.delta_time())};
  auto& transform {mCylinder.get<Transform>()};
  transform.rotate(2.0f * dt, 0.0f, 0.0f);

  mLightAngle += 20.0f / 7.0f * dt;
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  mScene->clear_directional_lights();

  const Vector3f32 lightDir {std::cos(mLightAngle), 1.0f,
                             std::sin(mLightAngle)};
  mScene->add_directional_light(Vector3f32::normalize(lightDir), Colors::WHITE);

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

} // namespace d3d9
