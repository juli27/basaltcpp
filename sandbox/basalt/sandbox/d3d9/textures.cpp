#include <basalt/sandbox/d3d9/textures.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/math/constants.h>

#include <basalt/api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using namespace entt::literals;

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
using basalt::gfx::Texture;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Textures::Textures(Engine& engine)
  : mSceneView {std::make_shared<SceneView>(mScene, create_default_camera())} {
  mScene->set_background(Colors::BLUE);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
    f32 u {};
    f32 v {};
  };

  array<Vertex, 50u * 2> vertices {};

  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};

    auto& vertex1 = vertices[2 * i];
    vertex1.x = sinTheta;
    vertex1.y = -1.0f;
    vertex1.z = cosTheta;
    vertex1.color = ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255);
    vertex1.u = i / (50.0f - 1);
    vertex1.v = 1.0f;

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.color = ColorEncoding::pack_a8r8g8b8_u32(128, 128, 128);
    vertex2.u = i / (50.0f - 1);
    vertex2.v = 0.0f;
  }

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuseA8R8G8B8_U32,
                                   VertexElement::TextureCoords2F32};

  const MeshDescriptor mesh {as_bytes(gsl::span {vertices}), vertexLayout,
                             PrimitiveType::TriangleStrip,
                             static_cast<u32>(vertices.size() - 2)};

  auto& rc {mCylinder.emplace<RenderComponent>()};
  rc.mesh = engine.gfx_resource_cache().create_mesh(mesh);

  MaterialDescriptor material;
  material.cullBackFace = false;
  material.lit = false;
  material.sampledTexture.texture =
    engine.get_or_load<Texture>("data/banana.bmp"_hs);

  rc.material = engine.gfx_resource_cache().create_material(material);
}

auto Textures::name() -> string_view {
  return "Tutorial 5: Using Texture Maps"sv;
}

auto Textures::drawable() -> basalt::gfx::DrawablePtr {
  return mSceneView;
}

void Textures::tick(Engine& engine) {
  auto& transform {mCylinder.get<Transform>()};
  transform.rotate(static_cast<f32>(engine.delta_time()), 0.0f, 0.0f);

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

auto Textures::do_handle_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::No;
}

} // namespace d3d9
