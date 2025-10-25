#include "samples.h"

#include <basalt/sandbox/shared/debug_scene_view.h>
#include <basalt/sandbox/shared/rotation_system.h>

#include <basalt/api/engine.h>
#include <basalt/api/scene_view.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>

using namespace basalt;
using namespace std::literals;

namespace {

constexpr auto MODEL_FILE_PATH = "data/Tiger.x"sv;

} // namespace

auto Samples::new_d3dx_x_mesh_sample(Engine& engine) -> ViewPtr {
  auto scene = Scene::create();
  scene->create_system<RotationSystem>();

  auto& sceneCtx = scene->entity_registry().ctx();
  auto& gfxEnv = sceneCtx.emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));
  gfxEnv.set_ambient_light(Colors::WHITE);

  auto const camera =
    scene->create_entity("Camera"s, Vector3f32{0.0f, 3.0f, -5.0f});
  camera.emplace<gfx::Camera>(Vector3f32::zero(), Vector3f32::up(), 45_deg,
                              1.0f, 100.0f);

  auto gfxCache = engine.create_gfx_resource_cache();

  {
    auto const tiger = scene->create_entity("Tiger"s);
    tiger.emplace<RotationSpeed>(0.0f, 1.0f, 0.0f);

    auto const modelData = gfxCache->load_x_meshes(MODEL_FILE_PATH);
    auto const material = [&] {
      auto const& material = modelData.materials.front();

      auto info = gfx::MaterialCreateInfo{};
      info.clazz = [&] {
        auto info = gfx::MaterialClassCreateInfo{};
        auto& pipelineInfo = info.pipelineInfo;

        auto vs = gfx::FixedVertexShaderCreateInfo{};
        vs.lightingEnabled = true;

        auto fs = gfx::FixedFragmentShaderCreateInfo{};
        auto textureStages = std::array{gfx::TextureStage{}};
        fs.textureStages = textureStages;

        pipelineInfo.vertexShader = &vs;
        pipelineInfo.fragmentShader = &fs;
        pipelineInfo.cullMode = gfx::CullMode::CounterClockwise;
        pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
        pipelineInfo.depthWriteEnable = true;

        return gfxCache->create_material_class(info);
      }();
      auto const properties = std::array{
        gfx::MaterialProperty{
          gfx::MaterialPropertyId::UniformColors,
          gfx::UniformColors{material.diffuse, material.ambient}},
        gfx::MaterialProperty{
          gfx::MaterialPropertyId::SampledTexture,
          gfx::SampledTexture{gfxCache->create_sampler({}),
                              gfxCache->load_texture_2d(material.textureFile)}},
      };
      info.initialValues = properties;

      return gfxCache->create_material(info);
    }();

    tiger.emplace<gfx::ext::XModel>(material, modelData.meshes.front());
  }

  return DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                camera.entity());
}
