#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/win32/shared/utils.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/utils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

using std::array;
using std::optional;
using std::string_view;

using Microsoft::WRL::ComPtr;

namespace basalt::gfx {
namespace {

constexpr auto to_d3d_color_value(const Color& color) noexcept
  -> D3DCOLORVALUE {
  return D3DCOLORVALUE {color.red(), color.green(), color.blue(),
                        color.alpha()};
}

constexpr auto to_d3d_matrix(const Mat4f32& mat) noexcept -> D3DMATRIX {
  // clang-format off
  return D3DMATRIX {mat.m11, mat.m12, mat.m13, mat.m14,
                    mat.m21, mat.m22, mat.m23, mat.m24,
                    mat.m31, mat.m32, mat.m33, mat.m34,
                    mat.m41, mat.m42, mat.m43, mat.m44};
  // clang-format on
}

constexpr auto to_d3d_vector(const Vector3f32& vec) noexcept -> D3DVECTOR {
  return D3DVECTOR {vec.x(), vec.y(), vec.z()};
}

auto to_fvf(const VertexLayout& layout) -> DWORD;

#if BASALT_DEV_BUILD

auto verify_fvf(DWORD fvf) -> bool;

#endif

auto to_d3d(const CullMode mode) -> D3DCULL {
  static constexpr EnumArray<CullMode, D3DCULL, 3> TO_D3D {
    {CullMode::None, D3DCULL_NONE},
    {CullMode::Clockwise, D3DCULL_CW},
    {CullMode::CounterClockwise, D3DCULL_CCW},
  };

  static_assert(CULL_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

auto to_d3d(const FillMode mode) -> D3DFILLMODE {
  static constexpr EnumArray<FillMode, D3DFILLMODE, 3> TO_D3D {
    {FillMode::Point, D3DFILL_POINT},
    {FillMode::Wireframe, D3DFILL_WIREFRAME},
    {FillMode::Solid, D3DFILL_SOLID},
  };

  static_assert(FILL_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

auto to_d3d(const ShadeMode mode) -> D3DSHADEMODE {
  static constexpr EnumArray<ShadeMode, D3DSHADEMODE, 2> TO_D3D {
    {ShadeMode::Flat, D3DSHADE_FLAT},
    {ShadeMode::Gouraud, D3DSHADE_GOURAUD},
  };

  static_assert(SHADE_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

// TODO: is there a benefit to turn off z testing when func = Never?
auto to_d3d(const DepthTestPass function) -> D3DCMPFUNC {
  static constexpr EnumArray<DepthTestPass, D3DCMPFUNC, 8> TO_D3D {
    {DepthTestPass::Never, D3DCMP_NEVER},
    {DepthTestPass::IfEqual, D3DCMP_EQUAL},
    {DepthTestPass::IfNotEqual, D3DCMP_NOTEQUAL},
    {DepthTestPass::IfLess, D3DCMP_LESS},
    {DepthTestPass::IfLessEqual, D3DCMP_LESSEQUAL},
    {DepthTestPass::IfGreater, D3DCMP_GREATER},
    {DepthTestPass::IfGreaterEqual, D3DCMP_GREATEREQUAL},
    {DepthTestPass::Always, D3DCMP_ALWAYS},
  };

  static_assert(DEPTH_TEST_PASS_COUNT == TO_D3D.size());

  return TO_D3D[function];
}

auto to_d3d(const RenderState& rs) -> std::tuple<D3DRENDERSTATETYPE, DWORD> {
  static constexpr EnumArray<RenderStateType, D3DRENDERSTATETYPE, 7> TO_D3D {
    {RenderStateType::CullMode, D3DRS_CULLMODE},
    {RenderStateType::Ambient, D3DRS_AMBIENT},
    {RenderStateType::Lighting, D3DRS_LIGHTING},
    {RenderStateType::FillMode, D3DRS_FILLMODE},
    {RenderStateType::DepthTest, D3DRS_ZFUNC},
    {RenderStateType::DepthWrite, D3DRS_ZWRITEENABLE},
    {RenderStateType::ShadeMode, D3DRS_SHADEMODE},
  };

  static_assert(RENDER_STATE_COUNT == TO_D3D.size());

  const D3DRENDERSTATETYPE d3dRs = TO_D3D[rs.type()];

  const DWORD d3dValue = std::visit(
    [](auto&& value) -> DWORD {
      using T = std::decay_t<decltype(value)>;
      if constexpr (std::is_same_v<T, bool>) {
        return value ? TRUE : FALSE;
      } else if constexpr (std::disjunction_v<std::is_same<T, CullMode>,
                                              std::is_same<T, FillMode>,
                                              std::is_same<T, ShadeMode>,
                                              std::is_same<T, DepthTestPass>>) {
        return to_d3d(value);
      } else if constexpr (std::is_same_v<T, Color>) {
        return enum_cast(value.to_argb());
      } else {
        static_assert(false, "non-exhaustive visitor");

        return 0ul;
      }
    },
    rs.value());

  return {d3dRs, d3dValue};
}

auto to_d3d_texture_stage_state(TextureStageState state, u32 value)
  -> std::tuple<D3DTEXTURESTAGESTATETYPE, DWORD>;

auto to_d3d_primitive_type(PrimitiveType) -> D3DPRIMITIVETYPE;

struct D3D9ImGuiRenderer final : ext::DearImGuiRenderer {
  explicit D3D9ImGuiRenderer(ComPtr<IDirect3DDevice9>);

  static void execute(const ext::CommandRenderDearImGui&);

  void init() override;
  void shutdown() override;
  void new_frame() override;

private:
  ComPtr<IDirect3DDevice9> mDevice;
};

struct D3D9XModelSupport final : ext::XModelSupport {
  explicit D3D9XModelSupport(ComPtr<IDirect3DDevice9>);

  void execute(const ext::CommandDrawXModel&) const;

  auto load(string_view filePath) -> ext::XModel override;

private:
  using TexturePtr = ComPtr<IDirect3DTexture9>;

  struct Model {
    std::vector<D3DMATERIAL9> materials;
    std::vector<TexturePtr> textures;
    ComPtr<ID3DXMesh> mesh;
  };

  ComPtr<IDirect3DDevice9> mDevice;
  HandlePool<Model, ext::XModel> mModels;
};

} // namespace

D3D9Device::D3D9Device(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
  BASALT_ASSERT(mDevice);

  mExtensions[ext::ExtensionId::DearImGuiRenderer] =
    std::make_shared<D3D9ImGuiRenderer>(mDevice);

  mExtensions[ext::ExtensionId::XModelSupport] =
    std::make_shared<D3D9XModelSupport>(mDevice);

  D3D9CALL(mDevice->GetDeviceCaps(&mDeviceCaps));
}

auto D3D9Device::device() const -> ComPtr<IDirect3DDevice9> {
  return mDevice;
}

void D3D9Device::reset(D3DPRESENT_PARAMETERS& pp) const {
  ImGui_ImplDX9_InvalidateDeviceObjects();

  // TODO: test cooperative level (see D3D9Context::present)
  D3D9CALL(mDevice->Reset(&pp));

  ImGui_ImplDX9_CreateDeviceObjects();
}

void D3D9Device::begin_execution() const {
  D3D9CALL(mDevice->BeginScene());
}

#define EXECUTE(commandType)                                                   \
  case commandType::TYPE:                                                      \
    execute(cmd->as<commandType>());                                           \
    break

// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void D3D9Device::execute(const CommandList& cmdList) {
  for (const auto& cmd : cmdList.commands()) {
    switch (cmd->type) {
      EXECUTE(CommandClear);
      EXECUTE(CommandDraw);
      EXECUTE(CommandSetDirectionalLights);
      EXECUTE(CommandSetTransform);
      EXECUTE(CommandSetMaterial);
      EXECUTE(CommandSetRenderState);
      EXECUTE(CommandSetTexture);
      EXECUTE(CommandSetTextureStageState);

    case CommandType::ExtDrawXModel:
      std::static_pointer_cast<D3D9XModelSupport>(
        mExtensions[ext::ExtensionId::XModelSupport])
        ->execute(cmd->as<ext::CommandDrawXModel>());
      break;

    case CommandType::ExtRenderDearImGui:
      D3D9ImGuiRenderer::execute(cmd->as<ext::CommandRenderDearImGui>());
      break;
    }
  }

  // disable used lights
  for (u8 i = 0; i < mMaxLightsUsed; i++) {
    D3D9CALL(mDevice->LightEnable(i, FALSE));
  }

  // reset render states
  // TODO: use command block
  D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
  D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                         D3DTTFF_DISABLE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_AMBIENT, 0u));
  D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
  D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD));

  const auto identity = to_d3d_matrix(Mat4f32::identity());
  D3D9CALL(mDevice->SetTransform(D3DTS_TEXTURE0, &identity));
  D3D9CALL(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &identity));
  D3D9CALL(mDevice->SetTransform(D3DTS_VIEW, &identity));
  D3D9CALL(mDevice->SetTransform(D3DTS_PROJECTION, &identity));

  D3D9CALL(mDevice->SetTexture(0, nullptr));
  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
}

#undef EXECUTE

void D3D9Device::end_execution() const {
  D3D9CALL(mDevice->EndScene());
}

auto D3D9Device::create_vertex_buffer(const gsl::span<const std::byte> data,
                                      const VertexLayout& layout)
  -> VertexBuffer {
  BASALT_ASSERT(!data.empty());
  BASALT_ASSERT(data.size() <= std::numeric_limits<UINT>::max());
  const UINT size = static_cast<UINT>(data.size());

  BASALT_ASSERT(!layout.empty());
  const DWORD fvf = to_fvf(layout);
  BASALT_ASSERT(verify_fvf(fvf), "invalid fvf. Consult the log for details");

  const auto [handle, vertexBuffer] = mVertexBuffers.allocate();

  D3D9CALL(mDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, fvf,
                                       D3DPOOL_MANAGED,
                                       vertexBuffer.GetAddressOf(), nullptr));

  // upload vertex data
  void* vertexBufferData {};
  if (SUCCEEDED(vertexBuffer->Lock(0u, 0u, &vertexBufferData, 0u))) {
    std::copy(data.begin(), data.end(),
              static_cast<std::byte*>(vertexBufferData));
    D3D9CALL(vertexBuffer->Unlock());
  } else {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");
  }

  return handle;
}

auto D3D9Device::add_texture(const string_view filePath) -> Texture {
  const auto [handle, texture] = mTextures.allocate();

  const auto wideFilePath = create_wide_from_utf8(filePath);
  if (FAILED(::D3DXCreateTextureFromFileW(mDevice.Get(), wideFilePath.c_str(),
                                          texture.GetAddressOf()))) {
    throw std::runtime_error("loading texture file failed");
  }

  return handle;
}

auto D3D9Device::query_extension(const ext::ExtensionId id)
  -> optional<ext::ExtensionPtr> {
  if (const auto entry = mExtensions.find(id); entry != mExtensions.end()) {
    return entry->second;
  }

  return std::nullopt;
}

void D3D9Device::execute(const CommandClear& cmd) const {
  const DWORD clearColor {to_d3d_color(cmd.color)};

  D3D9CALL(mDevice->Clear(0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                          clearColor, 1.0f, 0u));
}

void D3D9Device::execute(const CommandDraw& cmd) const {
  auto* vertexBuffer = mVertexBuffers[cmd.vertexBuffer].Get();

  D3DVERTEXBUFFER_DESC desc {};
  D3D9CALL(vertexBuffer->GetDesc(&desc));

  D3D9CALL(mDevice->SetStreamSource(0u, vertexBuffer, 0u,
                                    ::D3DXGetFVFVertexSize(desc.FVF)));
  D3D9CALL(mDevice->SetFVF(desc.FVF));
  D3D9CALL(mDevice->DrawPrimitive(to_d3d_primitive_type(cmd.primitiveType),
                                  cmd.startVertex, cmd.primitiveCount));
}

void D3D9Device::execute(const CommandSetDirectionalLights& cmd) {
  const auto& directionalLights = cmd.directionalLights;
  BASALT_ASSERT(directionalLights.size() <= mDeviceCaps.MaxActiveLights,
                "the renderer doesn't support that many lights");

  mMaxLightsUsed =
    std::max(mMaxLightsUsed, static_cast<u8>(directionalLights.size()));

  DWORD lightIndex = 0u;
  for (const auto& light : directionalLights) {
    D3DLIGHT9 d3dLight {};
    d3dLight.Type = D3DLIGHT_DIRECTIONAL;
    d3dLight.Diffuse = to_d3d_color_value(light.diffuseColor);
    d3dLight.Ambient = to_d3d_color_value(light.ambientColor);
    d3dLight.Direction = to_d3d_vector(light.direction);

    D3D9CALL(mDevice->SetLight(lightIndex, &d3dLight));
    D3D9CALL(mDevice->LightEnable(lightIndex, TRUE));
    lightIndex++;
  }
}

void D3D9Device::execute(const CommandSetTransform& cmd) const {
  const auto transform = to_d3d_matrix(cmd.transform);

  switch (cmd.state) {
  case TransformState::Projection:
    BASALT_ASSERT(transform._34 >= 0,
                  "(3,4) can't be negative in a projection matrix");

    D3D9CALL(mDevice->SetTransform(D3DTS_PROJECTION, &transform));
    break;

  case TransformState::View:
    D3D9CALL(mDevice->SetTransform(D3DTS_VIEW, &transform));
    break;

  case TransformState::World:
    D3D9CALL(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &transform));
    break;

  case TransformState::Texture:
    D3D9CALL(mDevice->SetTransform(D3DTS_TEXTURE0, &transform));
    break;
  }
}

void D3D9Device::execute(const CommandSetMaterial& cmd) const {
  D3DMATERIAL9 material {};
  material.Diffuse = to_d3d_color_value(cmd.diffuse);
  material.Ambient = to_d3d_color_value(cmd.ambient);
  material.Emissive = to_d3d_color_value(cmd.emissive);
  D3D9CALL(mDevice->SetMaterial(&material));
}

void D3D9Device::execute(const CommandSetRenderState& cmd) const {
  const auto [renderState, value] = to_d3d(cmd.renderState);

  D3D9CALL(mDevice->SetRenderState(renderState, value));
}

void D3D9Device::execute(const CommandSetTexture& cmd) const {
  const auto& texture = mTextures[cmd.texture];
  D3D9CALL(mDevice->SetTexture(0, texture.Get()));
}

void D3D9Device::execute(const CommandSetTextureStageState& cmd) const {
  const auto [textureStageState, value] =
    to_d3d_texture_stage_state(cmd.state, cmd.value);

  D3D9CALL(mDevice->SetTextureStageState(0, textureStageState, value));
}

namespace {

auto to_fvf(const VertexLayout& layout) -> DWORD {
  DWORD fvf = 0u;

  for (const auto& element : layout) {
    switch (element) {
    case VertexElement::Position3F32:
      fvf |= D3DFVF_XYZ;
      break;

    case VertexElement::PositionTransformed4F32:
      fvf |= D3DFVF_XYZRHW;
      break;

    case VertexElement::Normal3F32:
      fvf |= D3DFVF_NORMAL;
      break;

    case VertexElement::ColorDiffuseA8R8G8B8_U32:
      fvf |= D3DFVF_DIFFUSE;
      break;

    case VertexElement::ColorSpecularA8R8G8B8_U32:
      fvf |= D3DFVF_SPECULAR;
      break;

    case VertexElement::TextureCoords2F32:
      fvf |= D3DFVF_TEX1;
      break;
    }
  }

  return fvf;
}

#if BASALT_DEV_BUILD

auto verify_fvf(const DWORD fvf) -> bool {
  if (fvf & D3DFVF_XYZRHW && (fvf & D3DFVF_XYZ || fvf & D3DFVF_NORMAL)) {
    BASALT_LOG_ERROR("can't use transformed positions with untransformed"
                     "positions or normals");
    return false;
  }

  return true;
}

#endif

auto to_d3d_texture_stage_state(const TextureStageState state, const u32 value)
  -> std::tuple<D3DTEXTURESTAGESTATETYPE, DWORD> {
  static constexpr std::array<D3DTEXTURESTAGESTATETYPE, 2>
    TEXTURE_STAGE_STATE_CONV = {
      /* CoordinateSource      */ D3DTSS_TEXCOORDINDEX,
      /* TextureTransformFlags */ D3DTSS_TEXTURETRANSFORMFLAGS,
    };
  static_assert(TEXTURE_STAGE_STATE_COUNT == TEXTURE_STAGE_STATE_CONV.size());

  const D3DTEXTURESTAGESTATETYPE textureStageState =
    TEXTURE_STAGE_STATE_CONV[enum_cast(state)];
  DWORD d3dValue = 0;

  switch (state) {
  case TextureStageState::CoordinateSource:
    d3dValue = value == TcsVertexPositionCameraSpace
                 ? D3DTSS_TCI_CAMERASPACEPOSITION
                 : D3DTSS_TCI_PASSTHRU;
    break;

  case TextureStageState::TextureTransformFlags:
    if ((value & ~TtfProjected) == TtfCount4) {
      d3dValue = D3DTTFF_COUNT4;
    }

    if (value & TtfProjected) {
      d3dValue |= D3DTTFF_PROJECTED;
    }
    break;
  }

  return {textureStageState, d3dValue};
}

auto to_d3d_primitive_type(const PrimitiveType primitiveType)
  -> D3DPRIMITIVETYPE {
  switch (primitiveType) {
  case PrimitiveType::PointList:
    return D3DPT_POINTLIST;

  case PrimitiveType::LineList:
    return D3DPT_LINELIST;

  case PrimitiveType::LineStrip:
    return D3DPT_LINESTRIP;

  case PrimitiveType::TriangleList:
    return D3DPT_TRIANGLELIST;

  case PrimitiveType::TriangleStrip:
    return D3DPT_TRIANGLESTRIP;

  case PrimitiveType::TriangleFan:
    return D3DPT_TRIANGLEFAN;
  }

  return D3DPT_FORCE_DWORD;
}

D3D9ImGuiRenderer::D3D9ImGuiRenderer(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
}

void D3D9ImGuiRenderer::execute(const ext::CommandRenderDearImGui&) {
  ImGui::Render();
  if (auto* drawData = ImGui::GetDrawData()) {
    ImGui_ImplDX9_RenderDrawData(drawData);
  }
}

void D3D9ImGuiRenderer::init() {
  ImGui_ImplDX9_Init(mDevice.Get());
}

void D3D9ImGuiRenderer::shutdown() {
  ImGui_ImplDX9_Shutdown();
}

void D3D9ImGuiRenderer::new_frame() {
  ImGui_ImplDX9_NewFrame();
}

D3D9XModelSupport::D3D9XModelSupport(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
}

void D3D9XModelSupport::execute(const ext::CommandDrawXModel& cmd) const {
  const auto& model = mModels[cmd.handle];
  for (DWORD i = 0; i < model.materials.size(); i++) {
    D3D9CALL(mDevice->SetMaterial(&model.materials[i]));
    D3D9CALL(mDevice->SetTexture(0, model.textures[i].Get()));

    model.mesh->DrawSubset(i);
  }

  D3D9CALL(mDevice->SetTexture(0, nullptr));
}

auto D3D9XModelSupport::load(const string_view filePath) -> ext::XModel {
  const auto [handle, model] = mModels.allocate();

  const auto wideFilePath = create_wide_from_utf8(filePath);

  ComPtr<ID3DXBuffer> materialBuffer;
  DWORD numMaterials {};
  auto hr =
    ::D3DXLoadMeshFromXW(wideFilePath.c_str(), D3DXMESH_MANAGED, mDevice.Get(),
                         nullptr, materialBuffer.GetAddressOf(), nullptr,
                         &numMaterials, model.mesh.GetAddressOf());
  BASALT_ASSERT(SUCCEEDED(hr));

  auto const* materials =
    static_cast<D3DXMATERIAL*>(materialBuffer->GetBufferPointer());
  model.materials.reserve(numMaterials);
  model.textures.resize(numMaterials);

  for (DWORD i = 0; i < numMaterials; i++) {
    model.materials.emplace_back(materials->MatD3D);

    // d3dx doesn't set the ambient color
    model.materials[i].Ambient = model.materials[i].Diffuse;

    if (materials[i].pTextureFilename != nullptr &&
        lstrlenA(materials[i].pTextureFilename) > 0) {
      array<char, MAX_PATH> texPath {};
      strcpy_s(texPath.data(), texPath.size(), "data/");
      strcat_s(texPath.data(), texPath.size(), materials[i].pTextureFilename);

      hr = ::D3DXCreateTextureFromFileA(mDevice.Get(), texPath.data(),
                                        model.textures[i].GetAddressOf());
      BASALT_ASSERT(SUCCEEDED(hr));
    }
  }

  return handle;
}

} // namespace

} // namespace basalt::gfx
