#include "d3d9_tutorials.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <array>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;
using std::array;
using std::vector;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::View;
using basalt::ViewPtr;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::MaterialHandle;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::ResourceCachePtr;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::ext::XMeshCommandEncoder;
using basalt::gfx::ext::XMeshHandle;

namespace {

[[nodiscard]]
auto create_world_to_view_transform() noexcept -> Matrix4x4f32 {
  return Matrix4x4f32::look_at_lh({0.0f, 3.0f, -5.0f}, {}, Vector3f32::up());
}

[[nodiscard]]
auto create_view_to_clip_transform(basalt::Size2Du16 const viewport) noexcept
  -> Matrix4x4f32 {
  return Matrix4x4f32::perspective_projection(45_deg, viewport.aspect_ratio(),
                                              1.0f, 100.0f);
}

class Meshes final : public View {
  static constexpr auto sModelFilePath = "data/Tiger.x"sv;

public:
  explicit Meshes(Engine const& engine)
    : mGfxCache{engine.create_gfx_resource_cache()} {
    auto modelData = mGfxCache->load_x_meshes(sModelFilePath);
    mMaterials = [&] {
      auto materials = vector<MaterialHandle>{};

      auto const pipeline = [&] {
        auto vs = FixedVertexShaderCreateInfo{};
        vs.lightingEnabled = true;

        auto fs = FixedFragmentShaderCreateInfo{};
        auto textureStages = array{TextureStage{}};
        fs.textureStages = textureStages;

        auto pipelineDesc = PipelineCreateInfo{};
        pipelineDesc.vertexShader = &vs;
        pipelineDesc.fragmentShader = &fs;
        pipelineDesc.cullMode = CullMode::CounterClockwise;
        pipelineDesc.depthTest = TestPassCond::IfLessEqual;
        pipelineDesc.depthWriteEnable = true;

        return mGfxCache->create_pipeline(pipelineDesc);
      }();

      for (auto i = 0; i < modelData.materials.size(); i++) {
        auto const& material = modelData.materials[i];
        auto materialDesc = MaterialCreateInfo{};
        materialDesc.pipeline = pipeline;
        materialDesc.diffuse = material.diffuse;
        materialDesc.ambient = material.ambient;

        if (!material.textureFile.empty()) {
          materialDesc.sampledTexture.texture =
            mGfxCache->load_texture_2d(material.textureFile);
          materialDesc.sampledTexture.sampler = mGfxCache->create_sampler({});
        }

        materials.push_back(mGfxCache->create_material(materialDesc));
      }

      return materials;
    }();
    mMeshes = std::move(modelData.meshes);
  }

private:
  ResourceCachePtr mGfxCache;
  vector<MaterialHandle> mMaterials;
  vector<XMeshHandle> mMeshes;
  Angle mRotationY;

  auto on_update(UpdateContext& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    mRotationY += Angle::radians(dt);

    auto const& gfxCtx = ctx.engine.gfx_context();

    auto cmdList = CommandList{};
    cmdList.clear_attachments(
      Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
      Colors::BLUE, 1);
    cmdList.set_ambient_light(Colors::WHITE);
    cmdList.set_lights({});
    cmdList.set_transform(TransformState::ViewToClip,
                          create_view_to_clip_transform(ctx.drawCtx.viewport));
    cmdList.set_transform(TransformState::WorldToView,
                          create_world_to_view_transform());

    for (auto i = uSize{0}; i < mMaterials.size(); ++i) {
      auto const& materialData = gfxCtx.get(mMaterials[i]);

      cmdList.bind_pipeline(materialData.pipeline);
      cmdList.bind_sampler(0, materialData.sampler);
      cmdList.bind_texture(0, materialData.texture);
      cmdList.set_material(materialData.diffuse, materialData.ambient);
      cmdList.set_transform(TransformState::LocalToWorld,
                            Matrix4x4f32::rotation_y(mRotationY));
      XMeshCommandEncoder::draw_x_mesh(cmdList, mMeshes[i]);
    }

    ctx.drawCtx.commandLists.push_back(std::move(cmdList));
  }
};

} // namespace

auto D3D9Tutorials::new_meshes_tutorial(Engine& engine) -> ViewPtr {
  return std::make_shared<Meshes>(engine);
}
