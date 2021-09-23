#include <basalt/sandbox/d3d9/matrices.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/scene/transform.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/shared/config.h>

#include <array>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::PI;
using basalt::Transform;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Matrices::Matrices(Engine& engine)
  : mSceneView {std::make_shared<SceneView>(mScene, create_default_camera())} {
  mScene->set_background(Colors::BLACK);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
  };

  const array<Vertex, 3u> vertices {
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
    Vertex {0.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255)}};

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuseA8R8G8B8_U32};

  const MeshDescriptor mesh {as_bytes(gsl::span {vertices}), vertexLayout,
                             PrimitiveType::TriangleList,
                             static_cast<u32>(vertices.size() / 3)};

  auto& rc {mTriangle.emplace<RenderComponent>()};
  rc.mesh = engine.gfx_resource_cache().create_mesh(mesh);

  MaterialDescriptor material;
  material.cullBackFace = false;
  material.lit = false;

  rc.material = engine.gfx_resource_cache().create_material(material);
}

auto Matrices::name() -> string_view {
  return "Tutorial 3: Using Matrices"sv;
}

auto Matrices::drawable() -> basalt::gfx::DrawablePtr {
  return mSceneView;
}

void Matrices::tick(Engine& engine) {
  // 1 full rotation per second
  const f32 radOffsetY {2.0f * PI * static_cast<f32>(engine.delta_time())};
  auto& transform {mTriangle.get<Transform>()};
  transform.rotate(0.0f, radOffsetY, 0.0f);

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

auto Matrices::do_handle_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::No;
}

} // namespace d3d9
