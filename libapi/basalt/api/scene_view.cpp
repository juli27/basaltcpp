#include <basalt/api/scene_view.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>

#include <basalt/api/gfx/filtering_command_list.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/shared/config.h>

#include <basalt/api/math/matrix4x4.h>

#include <entt/entity/registry.hpp>

#include <string>
#include <utility>

namespace basalt {

using namespace std::literals;

using gfx::Attachment;
using gfx::Attachments;
using gfx::Camera;
using gfx::FilteringCommandList;
using gfx::Light;
using gfx::MaterialData;
using gfx::RenderComponent;
using gfx::TransformState;

namespace ext = gfx::ext;

namespace {

void record_material(FilteringCommandList& cmdList, const MaterialData& data) {
  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(data.texture);
  cmdList.bind_sampler(data.sampler);

  cmdList.set_material(data.diffuse, data.ambient, Color {});
}

} // namespace

SceneView::SceneView(ScenePtr scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::camera() const noexcept -> const Camera& {
  return mCamera;
}

auto SceneView::on_draw(const DrawContext& context) -> void {
  FilteringCommandList cmdList {};
  cmdList.clear_attachments(
    Attachments {Attachment::Color, Attachment::ZBuffer}, mScene->background(),
    1.0f, 0);

  cmdList.set_transform(TransformState::ViewToViewport,
                        mCamera.view_to_viewport(context.viewport));
  cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());

  cmdList.set_ambient_light(mScene->ambient_light());

  const auto& directionalLights {mScene->directional_lights()};
  if (!directionalLights.empty()) {
    const std::vector<Light> lights {directionalLights.begin(),
                                     directionalLights.end()};
    cmdList.set_lights(lights);
  }

  const auto& cache {context.cache};
  const auto& ecs {mScene->ecs()};

  ecs.view<const Transform, const ext::XModel>().each(
    [&](const Transform& transform, const ext::XModel& model) {
      const auto objToWorldMatrix {
        Matrix4x4f32::scaling(transform.scale) *
        Matrix4x4f32::rotation(transform.rotation) *
        Matrix4x4f32::translation(transform.position)};
      cmdList.set_transform(TransformState::ModelToWorld, objToWorldMatrix);

      const auto& modelData {cache.get(model)};

      const auto numMaterials {static_cast<u32>(modelData.materials.size())};

      for (u32 i {0}; i < numMaterials; ++i) {
        const auto& materialData {cache.get(modelData.materials[i])};
        record_material(cmdList, materialData);

        cmdList.ext_draw_x_mesh(modelData.mesh, i);
      }
    });

  ecs.view<const Transform, const RenderComponent>().each(
    [&](const Transform& transform, const RenderComponent& renderComponent) {
      const MaterialData& materialData {cache.get(renderComponent.material)};
      record_material(cmdList, materialData);

      cmdList.set_transform(TransformState::Texture,
                            renderComponent.texTransform);

      const auto objToWorldMatrix {
        Matrix4x4f32::scaling(transform.scale) *
        Matrix4x4f32::rotation(transform.rotation) *
        Matrix4x4f32::translation(transform.position)};
      cmdList.set_transform(TransformState::ModelToWorld, objToWorldMatrix);

      const auto& meshData = cache.get(renderComponent.mesh);
      cmdList.bind_vertex_buffer(meshData.vertexBuffer, 0ull);
      cmdList.draw(meshData.startVertex, meshData.vertexCount);
    });

  context.commandLists.push_back(cmdList.take_cmd_list());
}

void SceneView::on_tick(Engine& engine) {
  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

} // namespace basalt
