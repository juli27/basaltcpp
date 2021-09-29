#include <basalt/sandbox/d3d9/textures_tci.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/surface.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/mat4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using namespace entt::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::TexCoordinateSrc;
using basalt::gfx::Texture;
using basalt::gfx::TextureCoordinateSource;
using basalt::gfx::TextureTransformMode;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

struct MeshBuilder final {};

TexturesTci::TexturesTci(Engine& engine) {
  mScene->set_background(Colors::BLUE);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
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

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.color = ColorEncoding::pack_a8r8g8b8_u32(128, 128, 128);
  }

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuse1U32A8R8G8B8};

  auto& rc = mCylinder.emplace<RenderComponent>();

  MeshDescriptor mesh;
  mesh.data = as_bytes(gsl::span {vertices});
  mesh.layout = vertexLayout;
  mesh.primitiveType = PrimitiveType::TriangleStrip;
  mesh.primitiveCount = static_cast<u32>(vertices.size() - 2);

  rc.mesh = engine.gfx_resource_cache().create_mesh(mesh);

  MaterialDescriptor material;
  material.cullBackFace = false;
  material.lit = false;
  material.sampledTexture.texture =
    engine.get_or_load<Texture>("data/banana.bmp"_hs);
  material.textureCoordinateSource =
    TextureCoordinateSource::VertexPositionCameraSpace;
  material.textureTransformMode = TextureTransformMode::Count4;
  material.textureTransformProjected = true;

  rc.material = engine.gfx_resource_cache().create_material(material);

  const auto& camera = mSceneView->camera();
  rc.texTransform = camera.projection_matrix(engine.window_surface_size()) *
                    Mat4f32::scaling(Vector3f32 {0.5f, -0.5f, 1.0f}) *
                    Mat4f32::translation(Vector3f32 {0.5f, 0.5f, 0.0f});
}

auto TexturesTci::name() -> string_view {
  return "Tutorial 5: Using Texture Maps (TCI)"sv;
}

auto TexturesTci::drawable() -> basalt::gfx::DrawablePtr {
  return mSceneView;
}

void TexturesTci::tick(Engine& engine) {
  mCylinder.get<Transform>().rotate(static_cast<f32>(engine.delta_time()), 0.0f,
                                    0.0f);

  // update the texture transform, since it depends on the draw target size
  auto& rc = mCylinder.get<RenderComponent>();
  rc.texTransform =
    mSceneView->camera().projection_matrix(engine.window_surface_size()) *
    Mat4f32::scaling(Vector3f32 {0.5f, -0.5f, 1.0f}) *
    Mat4f32::translation(Vector3f32 {0.5f, 0.5f, 0.0f});

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

} // namespace d3d9
