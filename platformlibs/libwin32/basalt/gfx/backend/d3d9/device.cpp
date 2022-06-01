#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>

#include <gsl/span>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <algorithm>
#include <limits>
#include <new>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

using namespace std::literals;

using std::bad_alloc;
using std::optional;
using std::string_view;
using std::vector;
using std::filesystem::path;

using gsl::span;

using Microsoft::WRL::ComPtr;

namespace basalt::gfx {
namespace {

constexpr auto to_color(const D3DCOLORVALUE& color) -> Color {
  return Color::from_non_linear(color.r, color.g, color.b, color.a);
}

constexpr auto to_d3d_color_value(const Color& color) noexcept
  -> D3DCOLORVALUE {
  return D3DCOLORVALUE {color.r(), color.g(), color.b(), color.a()};
}

constexpr auto to_d3d(const Matrix4x4f32& mat) noexcept -> D3DMATRIX {
  // clang-format off
  return D3DMATRIX {mat.m11, mat.m12, mat.m13, mat.m14,
                    mat.m21, mat.m22, mat.m23, mat.m24,
                    mat.m31, mat.m32, mat.m33, mat.m34,
                    mat.m41, mat.m42, mat.m43, mat.m44};
  // clang-format on
}

constexpr auto to_d3d(const Vector3f32& vec) noexcept -> D3DVECTOR {
  return D3DVECTOR {vec.x(), vec.y(), vec.z()};
}

// TODO: needs some form of validation
auto to_fvf(const span<const VertexElement> layout) -> DWORD {
  DWORD fvf {0ul};

  // TODO: values >= 8 invalidate the fvf
  i32 numTexCoords {0};

  for (const VertexElement& element : layout) {
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

    case VertexElement::PointSize1F32:
      fvf |= D3DFVF_PSIZE;
      break;

    case VertexElement::ColorDiffuse1U32A8R8G8B8:
      fvf |= D3DFVF_DIFFUSE;
      break;

    case VertexElement::ColorSpecular1U32A8R8G8B8:
      fvf |= D3DFVF_SPECULAR;
      break;

    case VertexElement::TextureCoords1F32:
      fvf |= D3DFVF_TEXCOORDSIZE1(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords2F32:
      fvf |= D3DFVF_TEXCOORDSIZE2(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords3F32:
      fvf |= D3DFVF_TEXCOORDSIZE3(numTexCoords);
      ++numTexCoords;
      break;

    case VertexElement::TextureCoords4F32:
      fvf |= D3DFVF_TEXCOORDSIZE4(numTexCoords);
      ++numTexCoords;
      break;
    }
  }

  fvf |= std::min(8, numTexCoords) << D3DFVF_TEXCOUNT_SHIFT;

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

constexpr auto to_d3d(const bool b) noexcept -> BOOL {
  return b;
}

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

auto to_d3d(const PrimitiveType primitiveType) -> D3DPRIMITIVETYPE {
  static constexpr EnumArray<PrimitiveType, D3DPRIMITIVETYPE, 6> TO_D3D {
    {PrimitiveType::PointList, D3DPT_POINTLIST},
    {PrimitiveType::LineList, D3DPT_LINELIST},
    {PrimitiveType::LineStrip, D3DPT_LINESTRIP},
    {PrimitiveType::TriangleList, D3DPT_TRIANGLELIST},
    {PrimitiveType::TriangleStrip, D3DPT_TRIANGLESTRIP},
    {PrimitiveType::TriangleFan, D3DPT_TRIANGLEFAN},
  };
  static_assert(PRIMITIVE_TYPE_COUNT == TO_D3D.size());

  return TO_D3D[primitiveType];
}

auto to_d3d(const ShadeMode mode) -> D3DSHADEMODE {
  static constexpr EnumArray<ShadeMode, D3DSHADEMODE, 2> TO_D3D {
    {ShadeMode::Flat, D3DSHADE_FLAT},
    {ShadeMode::Gouraud, D3DSHADE_GOURAUD},
  };

  static_assert(SHADE_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

auto to_d3d(const TextureOp op) -> D3DTEXTUREOP {
  static constexpr EnumArray<TextureOp, D3DTEXTUREOP, 3> TO_D3D {
    {TextureOp::SelectArg1, D3DTOP_SELECTARG1},
    {TextureOp::SelectArg2, D3DTOP_SELECTARG2},
    {TextureOp::Modulate, D3DTOP_MODULATE},
  };
  static_assert(TEXTURE_OP_COUNT == TO_D3D.size());

  return TO_D3D[op];
}

auto to_d3d(const TextureCoordinateSource src) -> DWORD {
  static constexpr EnumArray<TextureCoordinateSource, DWORD, 2> TO_D3D {
    {TextureCoordinateSource::Vertex, D3DTSS_TCI_PASSTHRU},
    {TextureCoordinateSource::VertexPositionInView,
     D3DTSS_TCI_CAMERASPACEPOSITION},
  };
  static_assert(TEXTURE_COORDINATE_SOURCE_COUNT == TO_D3D.size());

  return TO_D3D[src];
}

auto to_d3d(const TextureStageArgument arg) -> DWORD {
  switch (arg) {
  case TextureStageArgument::Diffuse:
    return D3DTA_DIFFUSE;

  case TextureStageArgument::SampledTexture:
    return D3DTA_TEXTURE;
  }

  return 0u;
}

auto to_d3d(const TextureTransformMode mode) -> D3DTEXTURETRANSFORMFLAGS {
  static constexpr EnumArray<TextureTransformMode, D3DTEXTURETRANSFORMFLAGS, 2>
    TO_D3D {
      {TextureTransformMode::Disabled, D3DTTFF_DISABLE},
      {TextureTransformMode::Count4, D3DTTFF_COUNT4},
    };

  static_assert(TEXTURE_TRANSFORM_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

auto to_d3d(const TransformState state) -> D3DTRANSFORMSTATETYPE {
  static constexpr EnumArray<TransformState, D3DTRANSFORMSTATETYPE, 4> TO_D3D {
    {TransformState::ViewToViewport, D3DTS_PROJECTION},
    {TransformState::WorldToView, D3DTS_VIEW},
    {TransformState::ModelToWorld, D3DTS_WORLDMATRIX(0)},
    {TransformState::Texture, D3DTS_TEXTURE0},
  };

  static_assert(TRANSFORM_STATE_COUNT == TO_D3D.size());

  return TO_D3D[state];
}

// TODO: is there a benefit to turn off z testing when func = Never or func =
// Always with writing disabled?
auto to_d3d(const TestOp function) -> D3DCMPFUNC {
  static constexpr EnumArray<TestOp, D3DCMPFUNC, 8> TO_D3D {
    {TestOp::PassNever, D3DCMP_NEVER},
    {TestOp::PassIfEqual, D3DCMP_EQUAL},
    {TestOp::PassIfNotEqual, D3DCMP_NOTEQUAL},
    {TestOp::PassIfLess, D3DCMP_LESS},
    {TestOp::PassIfLessEqual, D3DCMP_LESSEQUAL},
    {TestOp::PassIfGreater, D3DCMP_GREATER},
    {TestOp::PassIfGreaterEqual, D3DCMP_GREATEREQUAL},
    {TestOp::PassAlways, D3DCMP_ALWAYS},
  };

  static_assert(DEPTH_TEST_PASS_COUNT == TO_D3D.size());

  return TO_D3D[function];
}

auto to_d3d(const TextureFilter filter) -> D3DTEXTUREFILTERTYPE {
  static constexpr EnumArray<TextureFilter, D3DTEXTUREFILTERTYPE, 3> TO_D3D {
    {TextureFilter::Point, D3DTEXF_POINT},
    {TextureFilter::Bilinear, D3DTEXF_LINEAR},
    {TextureFilter::Anisotropic, D3DTEXF_ANISOTROPIC},
  };

  static_assert(TEXTURE_FILTER_COUNT == TO_D3D.size());

  return TO_D3D[filter];
}

auto to_d3d(const TextureMipFilter filter) -> D3DTEXTUREFILTERTYPE {
  static constexpr EnumArray<TextureMipFilter, D3DTEXTUREFILTERTYPE, 3> TO_D3D {
    {TextureMipFilter::None, D3DTEXF_NONE},
    {TextureMipFilter::Point, D3DTEXF_POINT},
    {TextureMipFilter::Linear, D3DTEXF_LINEAR},
  };

  static_assert(TEXTURE_MIP_FILTER_COUNT == TO_D3D.size());

  return TO_D3D[filter];
}

auto to_d3d(const TextureAddressMode mode) -> D3DTEXTUREADDRESS {
  static constexpr EnumArray<TextureAddressMode, D3DTEXTUREADDRESS, 3> TO_D3D {
    {TextureAddressMode::Repeat, D3DTADDRESS_WRAP},
    {TextureAddressMode::Mirror, D3DTADDRESS_MIRROR},
    {TextureAddressMode::ClampToEdge, D3DTADDRESS_CLAMP},
  };

  static_assert(TEXTURE_ADDRESS_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

auto to_d3d(const PointLight& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_POINT;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuseColor);
  d3dLight.Ambient = to_d3d_color_value(light.ambientColor);
  d3dLight.Position = to_d3d(light.positionInWorld);
  d3dLight.Range = light.rangeInWorld;
  d3dLight.Attenuation0 = light.attenuation0;
  d3dLight.Attenuation1 = light.attenuation1;
  d3dLight.Attenuation2 = light.attenuation2;

  return d3dLight;
}

auto to_d3d(const SpotLight& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_SPOT;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuseColor);
  d3dLight.Ambient = to_d3d_color_value(light.ambientColor);
  d3dLight.Position = to_d3d(light.positionInWorld);
  d3dLight.Direction = to_d3d(light.directionInWorld);
  d3dLight.Range = light.rangeInWorld;
  d3dLight.Falloff = light.falloff;
  d3dLight.Attenuation0 = light.attenuation0;
  d3dLight.Attenuation1 = light.attenuation1;
  d3dLight.Attenuation2 = light.attenuation2;
  d3dLight.Theta = light.theta;
  d3dLight.Phi = light.phi;

  return d3dLight;
}

auto to_d3d(const DirectionalLight& light) -> D3DLIGHT9 {
  D3DLIGHT9 d3dLight {};
  d3dLight.Type = D3DLIGHT_DIRECTIONAL;
  d3dLight.Diffuse = to_d3d_color_value(light.diffuseColor);
  d3dLight.Ambient = to_d3d_color_value(light.ambientColor);
  d3dLight.Direction = to_d3d(light.direction);

  return d3dLight;
}

auto map_vertex_buffer(IDirect3DVertexBuffer9& vertexBuffer,
                       const uDeviceSize offset, uDeviceSize size)
  -> gsl::span<std::byte> {
  D3DVERTEXBUFFER_DESC desc {};
  D3D9CHECK(vertexBuffer.GetDesc(&desc));

  BASALT_ASSERT(offset < desc.Size);
  BASALT_ASSERT(offset + size <= desc.Size);

  if (size == 0) {
    size = desc.Size - offset;
  }

  if (offset >= desc.Size || size + offset > desc.Size) {
    BASALT_LOG_ERROR(
      "invalid map params: offset = {} size = {} (bufferSize = {})", offset,
      size, desc.Size);

    return {};
  }

  void* vertexBufferData {};
  // TODO: is offset in bytes or multiples of the stride
  if (FAILED(vertexBuffer.Lock(static_cast<UINT>(offset),
                               static_cast<UINT>(size), &vertexBufferData,
                               0ul))) {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");

    return {};
  }

  return {static_cast<std::byte*>(vertexBufferData), size};
}

auto calculate_primitive_count(const D3DPRIMITIVETYPE type,
                               const u32 vertexCount) -> u32 {
  switch (type) {
  case D3DPT_POINTLIST:
    return vertexCount;

  case D3DPT_LINELIST:
    return vertexCount / 2;

  case D3DPT_LINESTRIP:
    return vertexCount == 0 ? vertexCount : vertexCount - 1;

  case D3DPT_TRIANGLELIST:
    return vertexCount / 3;

  case D3DPT_TRIANGLESTRIP:
  case D3DPT_TRIANGLEFAN:
    return vertexCount < 3 ? 0 : vertexCount - 2;

  default:
    break;
  }

  return 0;
}

struct D3D9ImGuiRenderer final : ext::DearImGuiRenderer {
  explicit D3D9ImGuiRenderer(ComPtr<IDirect3DDevice9> device)
    : mDevice {std::move(device)} {
  }

  static auto execute(const ext::CommandRenderDearImGui&) -> void {
    ImGui::Render();
    if (auto* drawData {ImGui::GetDrawData()}) {
      ImGui_ImplDX9_RenderDrawData(drawData);
    }
  }

  void init() override {
    ImGui_ImplDX9_Init(mDevice.Get());
  }

  void shutdown() override {
    ImGui_ImplDX9_Shutdown();
  }

  void new_frame() override {
    ImGui_ImplDX9_NewFrame();
  }

private:
  ComPtr<IDirect3DDevice9> mDevice;
};

struct D3D9XModelSupport final : ext::XModelSupport {
  using DevicePtr = ComPtr<IDirect3DDevice9>;

  explicit D3D9XModelSupport(DevicePtr device) : mDevice {std::move(device)} {
  }

  void execute(const ext::CommandDrawXMesh& cmd) const {
    const auto& mesh {mMeshes[cmd.handle]};

    mesh->DrawSubset(cmd.subset);
  }

  [[nodiscard]] auto load(const path& filepath) -> ext::XModelData override {
    XMeshPtr mesh;

    ComPtr<ID3DXBuffer> materialBuffer;
    DWORD numMaterials {};
    if (FAILED(D3DXLoadMeshFromXW(filepath.c_str(), D3DXMESH_MANAGED,
                                  mDevice.Get(), nullptr, &materialBuffer,
                                  nullptr, &numMaterials, &mesh))) {
      throw std::runtime_error {"loading mesh file failed"};
    }

    vector<ext::XModelData::Material> materials {};

    materials.reserve(numMaterials);
    const auto* const d3dMaterials {
      static_cast<const D3DXMATERIAL*>(materialBuffer->GetBufferPointer())};

    for (DWORD i {0}; i < numMaterials; i++) {
      const auto& d3dMaterial {d3dMaterials[i].MatD3D};

      const Color diffuse {to_color(d3dMaterial.Diffuse)};
      auto& material {materials.emplace_back(ext::XModelData::Material {
        diffuse,
        diffuse, // d3dx doesn't set the ambient color
      })};

      if (!d3dMaterials[i].pTextureFilename) {
        continue;
      }

      const string_view texFileName {d3dMaterials[i].pTextureFilename};
      if (texFileName.empty()) {
        continue;
      }

      path texPath {"data"sv};
      texPath /= texFileName;
      material.textureFile = std::move(texPath);
    }

    const ext::XMesh meshHandle {mMeshes.allocate(std::move(mesh))};
    return ext::XModelData {meshHandle, std::move(materials)};
  }

  auto destroy(const ext::XMesh handle) noexcept -> void override {
    mMeshes.deallocate(handle);
  }

private:
  using XMeshPtr = ComPtr<ID3DXMesh>;

  DevicePtr mDevice;
  HandlePool<XMeshPtr, ext::XMesh> mMeshes;
};

} // namespace

D3D9Device::D3D9Device(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
  BASALT_ASSERT(mDevice);

  mExtensions[ext::ExtensionId::DearImGuiRenderer] =
    std::make_shared<D3D9ImGuiRenderer>(mDevice);

  mExtensions[ext::ExtensionId::XModelSupport] =
    std::make_shared<D3D9XModelSupport>(mDevice);

  D3DCAPS9 d3d9Caps {};
  D3D9CHECK(mDevice->GetDeviceCaps(&d3d9Caps));

  mCaps.maxVertexBufferSizeInBytes = std::numeric_limits<UINT>::max();
  mCaps.maxLights = d3d9Caps.MaxActiveLights;
  mCaps.maxTextureBlendStages = d3d9Caps.MaxTextureBlendStages;
  mCaps.maxBoundSampledTextures = d3d9Caps.MaxSimultaneousTextures;
  mCaps.maxTextureAnisotropy = d3d9Caps.MaxAnisotropy;
}

auto D3D9Device::device() const -> ComPtr<IDirect3DDevice9> {
  return mDevice;
}

auto D3D9Device::reset(D3DPRESENT_PARAMETERS& pp) const -> void {
  ImGui_ImplDX9_InvalidateDeviceObjects();

  // TODO: test cooperative level (see D3D9Context::present)
  D3D9CHECK(mDevice->Reset(&pp));

  ImGui_ImplDX9_CreateDeviceObjects();
}

auto D3D9Device::begin_execution() const -> void {
  // TODO: should this be a fatal error when failing?
  D3D9CHECK(mDevice->BeginScene());
}

// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
auto D3D9Device::execute(const CommandList& cmdList) -> void {
  // set device state to what the command list expects as default
  // TODO: use state block
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"set default state");

  mCurrentPrimitiveType = D3DPT_POINTLIST;

  D3D9CHECK(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
  D3D9CHECK(mDevice->SetTexture(0, nullptr));

  constexpr D3DMATRIX identity {to_d3d(Matrix4x4f32::identity())};
  D3D9CHECK(mDevice->SetTransform(D3DTS_PROJECTION, &identity));
  D3D9CHECK(mDevice->SetTransform(D3DTS_VIEW, &identity));
  D3D9CHECK(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &identity));
  D3D9CHECK(mDevice->SetTransform(D3DTS_TEXTURE0, &identity));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, 0u));

  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP));

  D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE));
  D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
  D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,
                                          0 | D3DTSS_TCI_PASSTHRU));
  D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                          D3DTTFF_DISABLE));

  PIX_END_EVENT();

  // overload resolution fails if one of the execute overloads is const
  // TODO: MSVC compiler bug?

  auto visitor {
    [&](auto&& cmd) { this->execute(std::forward<decltype(cmd)>(cmd)); }};

  std::for_each(cmdList.begin(), cmdList.end(),
                [&](const Command* cmd) { visit(*cmd, visitor); });

  // disable used lights
  for (u8 i = 0; i < mMaxLightsUsed; i++) {
    D3D9CHECK(mDevice->LightEnable(i, FALSE));
  }

  // unbind resources
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"unbind resources");
  D3D9CHECK(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
  D3D9CHECK(mDevice->SetTexture(0, nullptr));
  PIX_END_EVENT();
}

auto D3D9Device::end_execution() const -> void {
  D3D9CHECK(mDevice->EndScene());
}

auto D3D9Device::capabilities() const -> const DeviceCaps& {
  return mCaps;
}

auto D3D9Device::create_pipeline(const PipelineDescriptor& desc) -> Pipeline {
  BASALT_ASSERT(desc.textureStages.size() <= 1u);

  DWORD stage1Tci {0 | D3DTSS_TCI_PASSTHRU};
  D3DTEXTURETRANSFORMFLAGS stage0Ttf {D3DTTFF_DISABLE};
  DWORD stage1Arg1 {};
  DWORD stage1Arg2 {};
  D3DTEXTUREOP stage1ColorOp {D3DTOP_DISABLE};
  D3DTEXTUREOP stage1AlphaOp {D3DTOP_DISABLE};

  if (!desc.textureStages.empty()) {
    const TextureBlendingStage& stage {desc.textureStages[0]};
    stage1Tci = 0 | to_d3d(stage.texCoordinateSrc);
    stage0Ttf = to_d3d(stage.texCoordinateTransformMode);
    if (stage.texCoordinateProjected) {
      stage0Ttf =
        static_cast<D3DTEXTURETRANSFORMFLAGS>(stage0Ttf | D3DTTFF_PROJECTED);
    }
    stage1Arg1 = to_d3d(stage.arg1);
    stage1Arg2 = to_d3d(stage.arg2);
    stage1ColorOp = to_d3d(stage.colorOp);
    stage1AlphaOp = to_d3d(stage.alphaOp);
  }

  const DWORD fvf {to_fvf(desc.vertexInputState)};
  BASALT_ASSERT(verify_fvf(fvf), "invalid fvf. Consult the log for details");

  return mPipelines.allocate(PipelineData {
    fvf,
    stage1Tci,
    stage0Ttf,
    stage1Arg1,
    stage1Arg2,
    stage1ColorOp,
    stage1AlphaOp,
    to_d3d(desc.primitiveType),
    to_d3d(desc.lighting),
    to_d3d(desc.shadeMode),
    to_d3d(desc.cullMode),
    to_d3d(desc.fillMode),
    desc.depthTest == TestOp::PassAlways && !desc.depthWriteEnable ? D3DZB_FALSE
                                                                   : D3DZB_TRUE,
    to_d3d(desc.depthTest),
    to_d3d(desc.depthWriteEnable),
    to_d3d(desc.dithering),
  });
}

auto D3D9Device::destroy(const Pipeline handle) noexcept -> void {
  mPipelines.deallocate(handle);
}

// throws std::bad_alloc when requested size is too large and when d3d9
// allocation fails
auto D3D9Device::create_vertex_buffer(
  const VertexBufferDescriptor& desc,
  const gsl::span<const std::byte> initialData) -> VertexBuffer {
  BASALT_ASSERT(!desc.layout.empty());
  BASALT_ASSERT(initialData.size() <= desc.sizeInBytes);

  if (desc.sizeInBytes > mCaps.maxVertexBufferSizeInBytes) {
    BASALT_LOG_ERROR("requested vertex buffer size ({}) too large",
                     desc.sizeInBytes);

    throw bad_alloc {};
  }

  const DWORD fvf {to_fvf(desc.layout)};
  BASALT_ASSERT(verify_fvf(fvf), "invalid fvf. Consult the log for details");

  // FVF vertex buffers must be large enough to contain at least one vertex, but
  // it need not be a multiple of the vertex size
  const UINT minSize {D3DXGetFVFVertexSize(fvf)};
  BASALT_ASSERT(desc.sizeInBytes >= minSize,
                "vertex buffer must contain at least one vertex");

  const UINT size {std::max(minSize, static_cast<UINT>(desc.sizeInBytes))};

  D3D9VertexBufferPtr vertexBuffer {};
  if (FAILED(mDevice->CreateVertexBuffer(size, 0ul, fvf, D3DPOOL_MANAGED,
                                         &vertexBuffer, nullptr))) {
    BASALT_LOG_ERROR("failed to allocate vertex buffer");

    throw bad_alloc {};
  }

  if (!initialData.empty()) {
    // TODO: should initialData.size() > size be an error?
    // TODO: should failing to upload initial data be an error?
    if (const gsl::span vertexBufferData {
          map_vertex_buffer(*vertexBuffer.Get(), 0, 0)};
        !vertexBufferData.empty()) {
      std::copy_n(initialData.begin(),
                  std::min(initialData.size(), uSize {size}),
                  vertexBufferData.begin());

      D3D9CHECK(vertexBuffer->Unlock());
    }
  }

  return mVertexBuffers.allocate(std::move(vertexBuffer));
}

void D3D9Device::destroy(const VertexBuffer handle) noexcept {
  mVertexBuffers.deallocate(handle);
}

auto D3D9Device::map(const VertexBuffer handle, const uDeviceSize offset,
                     const uDeviceSize size) -> gsl::span<std::byte> {
  BASALT_ASSERT(mVertexBuffers.is_valid(handle));

  if (!mVertexBuffers.is_valid(handle)) {
    return {};
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  return map_vertex_buffer(*vertexBuffer.Get(), offset, size);
}

void D3D9Device::unmap(const VertexBuffer handle) noexcept {
  BASALT_ASSERT(mVertexBuffers.is_valid(handle));

  if (!mVertexBuffers.is_valid(handle)) {
    return;
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  D3D9CHECK(vertexBuffer->Unlock());
}

auto D3D9Device::load_texture(const path& filePath) -> Texture {
  D3D9TexturePtr texture;

  if (FAILED(D3DXCreateTextureFromFileExW(
        mDevice.Get(), filePath.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, nullptr, nullptr, &texture))) {
    throw std::runtime_error {"loading texture file failed"};
  }

  return mTextures.allocate(std::move(texture));
}

void D3D9Device::destroy(const Texture handle) noexcept {
  mTextures.deallocate(handle);
}

auto D3D9Device::create_sampler(const SamplerDescriptor& desc) -> Sampler {
  return mSamplers.allocate(SamplerData {
    to_d3d(desc.magFilter),
    to_d3d(desc.minFilter),
    to_d3d(desc.mipFilter),
    to_d3d(desc.addressModeU),
    to_d3d(desc.addressModeV),
    to_d3d(desc.addressModeW),
  });
}

auto D3D9Device::destroy(const Sampler handle) noexcept -> void {
  mSamplers.deallocate(handle);
}

auto D3D9Device::query_extension(const ext::ExtensionId id)
  -> optional<ext::ExtensionPtr> {
  if (const auto entry = mExtensions.find(id); entry != mExtensions.end()) {
    return entry->second;
  }

  return std::nullopt;
}

auto D3D9Device::execute(const Command& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh:
    std::static_pointer_cast<const D3D9XModelSupport>(
      mExtensions[ext::ExtensionId::XModelSupport])
      ->execute(cmd.as<ext::CommandDrawXMesh>());
    break;

  case CommandType::ExtRenderDearImGui:
    D3D9ImGuiRenderer::execute(cmd.as<ext::CommandRenderDearImGui>());
    break;

  default:
    BASALT_LOG_ERROR("d3d9 device can't handle this command");
  }
}

auto D3D9Device::execute(const CommandClearAttachments& cmd) -> void {
  BASALT_ASSERT_IF(cmd.attachments.has(Attachment::DepthBuffer),
                   cmd.depth >= 0.0f && cmd.depth <= 1.0f);

  const DWORD flags {[&] {
    DWORD f {0};

    if (cmd.attachments.has(Attachment::RenderTarget)) {
      f |= D3DCLEAR_TARGET;
    }

    if (cmd.attachments.has(Attachment::DepthBuffer)) {
      f |= D3DCLEAR_ZBUFFER;
    }

    if (cmd.attachments.has(Attachment::StencilBuffer)) {
      f |= D3DCLEAR_STENCIL;
    }

    return f;
  }()};

  D3D9CHECK(mDevice->Clear(0u, nullptr, flags, to_d3d(cmd.color), cmd.depth,
                           cmd.stencil));
}

auto D3D9Device::execute(const CommandDraw& cmd) -> void {
  D3D9CHECK(mDevice->DrawPrimitive(
    mCurrentPrimitiveType, cmd.firstVertex,
    calculate_primitive_count(mCurrentPrimitiveType, cmd.vertexCount)));
}

auto D3D9Device::execute(const CommandBindPipeline& cmd) -> void {
  BASALT_ASSERT(mPipelines.is_valid(cmd.handle));

  if (!mPipelines.is_valid(cmd.handle)) {
    return;
  }

  const PipelineData& data {mPipelines[cmd.handle]};
  mCurrentPrimitiveType = data.primitiveType;

  D3D9CHECK(mDevice->SetFVF(data.fvf));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_LIGHTING, data.lighting));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_SHADEMODE, data.shadeMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CULLMODE, data.cullMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FILLMODE, data.fillMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZENABLE, data.zEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZFUNC, data.zFunc));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, data.zWriteEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_DITHERENABLE, data.dithering));

  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, data.stage1ColorOp));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, data.stage1Arg1));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_COLORARG2, data.stage1Arg2));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, data.stage1AlphaOp));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, data.stage1Arg1));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, data.stage1Arg2));
  D3D9CHECK(
    mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, data.stage1Tci));
  D3D9CHECK(mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                          data.stage0Ttf));

  D3D9CHECK(mDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE));
  D3D9CHECK(mDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
}

auto D3D9Device::execute(const CommandBindVertexBuffer& cmd) -> void {
  BASALT_ASSERT(mVertexBuffers.is_valid(cmd.handle));

  if (!mVertexBuffers.is_valid(cmd.handle)) {
    return;
  }

  const D3D9VertexBufferPtr& buffer {mVertexBuffers[cmd.handle]};

  D3DVERTEXBUFFER_DESC desc {};
  D3D9CHECK(buffer->GetDesc(&desc));

  const UINT fvfStride {D3DXGetFVFVertexSize(desc.FVF)};
  const UINT maxOffset {desc.Size - fvfStride};
  BASALT_ASSERT(cmd.offset < maxOffset);

  const UINT offset {std::min(maxOffset, static_cast<UINT>(cmd.offset))};

  D3D9CHECK(mDevice->SetStreamSource(0u, buffer.Get(), offset, fvfStride));
}

auto D3D9Device::execute(const CommandBindSampler& cmd) -> void {
  BASALT_ASSERT(mSamplers.is_valid(cmd.sampler));

  if (!mSamplers.is_valid(cmd.sampler)) {
    return;
  }

  const auto& data {mSamplers[cmd.sampler]};

  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, data.minFilter));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, data.magFilter));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, data.mipFilter));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY,
                                     mCaps.maxTextureAnisotropy));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, data.addressModeU));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, data.addressModeV));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, data.addressModeW));
}

auto D3D9Device::execute(const CommandBindTexture& cmd) -> void {
  BASALT_ASSERT(mTextures.is_valid(cmd.texture));

  if (!mTextures.is_valid(cmd.texture)) {
    return;
  }

  const D3D9TexturePtr& texture {mTextures[cmd.texture]};

  D3D9CHECK(mDevice->SetTexture(0, texture.Get()));
}

auto D3D9Device::execute(const CommandSetTransform& cmd) -> void {
  BASALT_ASSERT_IF(cmd.state == TransformState::ViewToViewport,
                   cmd.transform.m34 >= 0,
                   "(3,4) can't be negative in a projection matrix");

  const D3DTRANSFORMSTATETYPE state {to_d3d(cmd.state)};
  const auto transform {to_d3d(cmd.transform)};

  D3D9CHECK(mDevice->SetTransform(state, &transform));
}

void D3D9Device::execute(const CommandSetAmbientLight& cmd) {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, to_d3d(cmd.ambientColor)));
}

void D3D9Device::execute(const CommandSetLights& cmd) {
  const auto& lights {cmd.lights};
  BASALT_ASSERT(lights.size() <= mCaps.maxLights,
                "the renderer doesn't support that many lights");

  mMaxLightsUsed = std::max(mMaxLightsUsed, static_cast<u8>(lights.size()));

  DWORD lightIndex {0ul};
  for (const auto& l : lights) {
    D3DLIGHT9 d3dLight {visit([](auto&& light) { return to_d3d(light); }, l)};

    D3D9CHECK(mDevice->SetLight(lightIndex, &d3dLight));
    D3D9CHECK(mDevice->LightEnable(lightIndex, TRUE));
    lightIndex++;
  }
}

void D3D9Device::execute(const CommandSetMaterial& cmd) {
  const D3DMATERIAL9 material {
    to_d3d_color_value(cmd.diffuse),
    to_d3d_color_value(cmd.ambient),
    D3DCOLORVALUE {},
    to_d3d_color_value(cmd.emissive),
    0.0f,
  };

  D3D9CHECK(mDevice->SetMaterial(&material));
}

} // namespace basalt::gfx
