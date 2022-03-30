#include <basalt/sandbox/d3d9/meshes.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/mat4.h>

#include <string_view>
#include <utility>

namespace d3d9 {

using namespace std::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::TransformState;

Meshes::Meshes(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()}
  , mModel {mResourceCache.load_x_model("data/Tiger.x"sv)}
  , mCamera {create_default_camera()} {
}

Meshes::~Meshes() noexcept {
  mResourceCache.destroy(mModel);
}

void Meshes::on_draw(const DrawContext& context) {
  CommandList cmdList {};

  cmdList.clear_attachments(
    Attachments {Attachment::Color, Attachment::ZBuffer}, Colors::BLUE, 1.0f,
    0);

  cmdList.set_ambient_light(Colors::WHITE);

  cmdList.set_transform(TransformState::ViewToViewport,
                        mCamera.projection_matrix(context.viewport));
  cmdList.set_transform(TransformState::WorldToView, mCamera.view_matrix());

  const auto& modelData {mResourceCache.get(mModel)};

  const u32 numMaterials {static_cast<u32>(modelData.materials.size())};
  for (u32 i {0}; i < numMaterials; ++i) {
    const auto& materialData {mResourceCache.get(modelData.materials[i])};

    cmdList.bind_pipeline(materialData.pipeline);

    cmdList.bind_sampler(materialData.sampler);
    cmdList.bind_texture(materialData.texture);
    cmdList.set_material(materialData.diffuse, materialData.ambient, Color {});

    cmdList.set_transform(TransformState::ModelToWorld,
                          Mat4f32::rotation_y(mRotationY));

    cmdList.ext_draw_x_mesh(modelData.mesh, i);
  }

  context.commandLists.push_back(std::move(cmdList));
}

void Meshes::on_tick(Engine& engine) {
  const auto dt {static_cast<f32>(engine.delta_time())};

  mRotationY += Angle::radians(dt);
  while (mRotationY.radians() > PI) {
    mRotationY -= Angle::radians(PI * 2.0f);
  }
}

} // namespace d3d9
