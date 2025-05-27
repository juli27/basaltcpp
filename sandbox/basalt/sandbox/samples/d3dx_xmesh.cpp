#include "samples.h"

#include "basalt/sandbox/shared/debug_scene_view.h"

#include <basalt/api/engine.h>
#include <basalt/api/scene_view.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::array;

using namespace basalt;

namespace {

constexpr auto MODEL_FILE_PATH = "data/Tiger.x"sv;

struct RotationSpeed {
  f32 radPerSecond;
};

class RotationSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  RotationSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    ctx.scene.entity_registry().view<Transform, RotationSpeed const>().each(
      [&](Transform& t, RotationSpeed const& rotationSpeed) {
        t.rotate_y(Angle::radians(rotationSpeed.radPerSecond * dt));
      });
  }
};

} // namespace

auto Samples::new_d3dx_x_mesh_sample(Engine& engine) -> ViewPtr {
  auto scene = Scene::create();
  scene->create_system<RotationSystem>();

  auto& sceneCtx = scene->entity_registry().ctx();
  auto& gfxEnv = sceneCtx.emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));
  gfxEnv.set_ambient_light(Colors::WHITE);

  auto camera = [&] {
    auto e = scene->create_entity("Camera"s, Vector3f32{0.0f, 3.0f, -5.0f});
    e.emplace<gfx::Camera>(Vector3f32::zero(), Vector3f32::up(), 45_deg, 1.0f,
                           100.0f);

    return e;
  }();

  auto gfxCache = engine.create_gfx_resource_cache();

  {
    auto tiger = scene->create_entity("Tiger"s);
    tiger.emplace<RotationSpeed>(1.0f);

    auto const modelData = gfxCache->load_x_meshes(MODEL_FILE_PATH);
    auto material = [&] {
      auto const& material = modelData.materials.front();

      auto info = gfx::MaterialCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;
      auto vs = gfx::FixedVertexShaderCreateInfo{};
      vs.lightingEnabled = true;

      auto fs = gfx::FixedFragmentShaderCreateInfo{};
      auto textureStages = array{gfx::TextureStage{}};
      fs.textureStages = textureStages;

      pipelineInfo.vertexShader = &vs;
      pipelineInfo.fragmentShader = &fs;
      pipelineInfo.cullMode = gfx::CullMode::CounterClockwise;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;
      info.pipeline = gfxCache->create_pipeline(pipelineInfo);

      info.diffuse = material.diffuse;
      info.ambient = material.ambient;

      info.sampledTexture.texture =
        gfxCache->load_texture_2d(material.textureFile);
      info.sampledTexture.sampler = gfxCache->create_sampler({});

      return gfxCache->create_material(info);
    }();

    tiger.emplace<gfx::ext::XModel>(material, modelData.meshes.front());
  }

  return DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                camera.entity());
}
