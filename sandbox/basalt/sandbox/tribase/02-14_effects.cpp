#include <basalt/sandbox/tribase/02-14_effects.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/effect.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector4.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

namespace tribase {

using namespace std::literals;
using std::array;
using std::byte;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::path;

using gsl::czstring;
using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector2f32;
using basalt::Vector4f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCachePtr;
using basalt::gfx::TestPassCond;
using basalt::gfx::Texture;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::Effect;
using basalt::gfx::ext::EffectCommandEncoder;
using basalt::gfx::ext::EffectId;
using basalt::gfx::ext::XMeshCommandEncoder;
using basalt::gfx::ext::XModel;

namespace {

struct Vertex {
  Vector4f32 pos{};
  Vector2f32 tex{};

  static constexpr auto sLayout = array{
    VertexElement::PositionTransformed4F32,
    VertexElement::TextureCoords2F32,
  };
};

constexpr auto RECT_VERTICES = array{
  Vertex{{0, 1, 1, 1}, {0, 1}},
  Vertex{{0, 0, 1, 1}, {0, 0}},
  Vertex{{1, 1, 1, 1}, {1, 1}},
  Vertex{{1, 0, 1, 1}, {1, 0}},
};

constexpr auto DATA_PATH = "data/tribase/02-14_effects"sv;
constexpr auto EFFECT_FILENAME = "Wire-Frame.fx"sv;
constexpr auto SPHERE_FILENAME = "Sphere.x"sv;

} // namespace

Effects::Effects(Engine const& engine)
  : mGfxCache{engine.gfx_context().create_resource_cache()}
  , mRectVb{mGfxCache->create_vertex_buffer(
      {RECT_VERTICES.size() * sizeof(Vertex), Vertex::sLayout})}
  , mLinearSampler{mGfxCache->create_sampler({TextureFilter::Bilinear,
                                              TextureFilter::Bilinear,
                                              TextureMipFilter::Linear})}
  , mBackgroundPipeline{[&] {
    constexpr auto textureStages = array{TextureStage{}};
    auto fs = FixedFragmentShaderCreateInfo{};
    fs.textureStages = textureStages;
    auto pipelineDesc = PipelineDescriptor{};
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.vertexLayout = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.cullMode = CullMode::CounterClockwise;
    pipelineDesc.dithering = true;

    return mGfxCache->create_pipeline(pipelineDesc);
  }()}
  , mDefaultPipeline{[&] {
    auto vs = FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;
    auto pipelineDesc = PipelineDescriptor{};
    pipelineDesc.vertexShader = &vs;
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.cullMode = CullMode::CounterClockwise;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;
    pipelineDesc.dithering = true;

    return mGfxCache->create_pipeline(pipelineDesc);
  }()} {
  list_effect_files();
  mEffectFilePath = mEffectFilePaths.at(0);
  mLoadedEffect = LoadedEffect::compile(mEffectFilePath, engine).value();
}

auto Effects::LoadedEffect::compile(path const& filePath, Engine const& engine)
  -> std::optional<LoadedEffect> {
  auto gfxCache = engine.create_gfx_resource_cache();
  auto const result = gfxCache->compile_effect(filePath);

  if (auto const* error = std::get_if<string>(&result)) {
    BASALT_LOG_ERROR("Failed to compile effect {}", filePath.u8string());
    BASALT_LOG_ERROR(*error);

    return std::nullopt;
  }

  auto const& id = std::get<EffectId>(result);
  auto& effect = gfxCache->get(id);

  auto description = [&] {
    if (auto maybeDescription = effect.get_string("Description")) {
      return *std::move(maybeDescription);
    }

    return string{EFFECT_FILENAME};
  }();

  auto const dataPath = filePath.parent_path();
  auto const backgroundTexture = [&] {
    if (auto const maybeBackgroundTexture =
          effect.get_string("BackgroundImage")) {
      return gfxCache->load_texture(dataPath / *maybeBackgroundTexture);
    }

    return Texture::null();
  }();

  auto const model = [&] {
    if (auto const maybeModelFileName = effect.get_string("ModelFilename")) {
      return gfxCache->load_x_model(dataPath / *maybeModelFileName);
    }

    return gfxCache->load_x_model(dataPath / SPHERE_FILENAME);
  }();

  auto textures = array<Texture, 4>{};
  for (auto i = i32{0}; i < 4; ++i) {
    auto const parameter = fmt::format(FMT_STRING("TextureFilename{}"), i + 1);
    if (auto const maybeTextureFilename =
          effect.get_string(parameter.c_str())) {
      textures[i] = gfxCache->load_texture(dataPath / *maybeTextureFilename);
    }
  }

  effect.set_texture("Texture1", textures[0]);
  effect.set_texture("Texture2", textures[1]);
  effect.set_texture("Texture3", textures[2]);
  effect.set_texture("Texture4", textures[3]);

  effect.set_technique([&] {
    // search for a valid technique
    for (auto i = u32{0}; i < effect.get_num_techniques(); ++i) {
      if (auto const handle = effect.get_technique(i);
          effect.validate_technique(handle)) {
        return handle;
      }
    }

    return effect.get_technique(0);
  }());

  auto const activeTechniqueName = effect.get_technique_name();
  auto const activeTechniqueNumPasses = effect.get_technique_num_passes();

  return LoadedEffect{std::move(gfxCache),     id,    std::move(description),
                      backgroundTexture,       model, activeTechniqueName,
                      activeTechniqueNumPasses};
}

auto Effects::list_effect_files() -> void {
  auto const dataPath = path{DATA_PATH};
  for (auto const& entry : directory_iterator{dataPath}) {
    if (auto const& entryPath = entry.path(); entryPath.extension() == ".fx") {
      mEffectFilePaths.push_back(entryPath);
    }
  }

  std::sort(mEffectFilePaths.begin(), mEffectFilePaths.end());
}

auto Effects::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();

  if (ImGui::Begin("Effect")) {
    ImGui::Text("Description: %s", mLoadedEffect.description.c_str());
    ImGui::Text("Technique: %s", mLoadedEffect.activeTechniqueName);

    if (ImGui::BeginListBox("##Effects", ImVec2{-1, 0})) {
      for (auto const& effectPath : mEffectFilePaths) {
        auto const selected = effectPath == mEffectFilePath;
        auto const fileName = effectPath.filename().u8string();
        if (ImGui::Selectable(fileName.c_str(), selected)) {
          if (auto newEffect = LoadedEffect::compile(effectPath, ctx.engine)) {
            mEffectFilePath = effectPath;
            mLoadedEffect = *std::move(newEffect);
          }
        }
      }

      ImGui::EndListBox();
    }
  }
  ImGui::End();

  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 0, 127), 1);

  if (mBackgroundTex) {
    mGfxCache->with_mapping_of(
      mRectVb, [viewport = ctx.drawCtx.viewport](span<byte> const vbData) {
        auto const vb = span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                                     vbData.size() / sizeof(Vertex)};
        std::copy(RECT_VERTICES.begin(), RECT_VERTICES.end(), vb.begin());

        for (auto& vertex : vb) {
          // !!! READING the mapped buffer !!!
          vertex.pos.x() *= static_cast<f32>(viewport.width());
          vertex.pos.y() *= static_cast<f32>(viewport.height());
        }
      });

    cmdList.bind_pipeline(mBackgroundPipeline);
    cmdList.bind_sampler(0, mLinearSampler);
    cmdList.bind_texture(0, mBackgroundTex);
    cmdList.bind_vertex_buffer(mRectVb);
    cmdList.draw(0, 4);
  }

  cmdList.bind_pipeline(mDefaultPipeline);
  cmdList.bind_sampler(0, mLinearSampler);
  cmdList.bind_sampler(1, mLinearSampler);
  cmdList.bind_sampler(2, mLinearSampler);
  cmdList.bind_sampler(3, mLinearSampler);
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(
      90_deg, ctx.drawCtx.viewport.aspect_ratio(), 0.1f, 10.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation(Angle::radians(t),
                                               Angle::radians(0.75f * t),
                                               Angle::radians(0.5f * t)) *
                          Matrix4x4f32::translation(0, 0, 2));

  EffectCommandEncoder::begin_effect(cmdList, mLoadedEffect.id);

  auto const modelData = mLoadedEffect.gfxCache->get(mLoadedEffect.model);
  for (auto i = u32{0}; i < mLoadedEffect.activeTechniqueNumPasses; ++i) {
    EffectCommandEncoder::begin_effect_pass(cmdList, i);
    XMeshCommandEncoder::draw_x_mesh(cmdList, modelData.meshes[0]);
    EffectCommandEncoder::end_effect_pass(cmdList);
  }

  EffectCommandEncoder::end_effect(cmdList);

  ctx.drawCtx.commandLists.push_back(std::move(cmdList));
}

} // namespace tribase
