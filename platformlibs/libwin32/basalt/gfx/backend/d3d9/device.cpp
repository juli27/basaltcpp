#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/win32/shared/utils.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/enum_array.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <algorithm>
#include <array>
#include <limits>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

using std::array;
using std::bad_alloc;
using std::optional;
using std::string_view;
using std::filesystem::path;

using Microsoft::WRL::ComPtr;

namespace basalt::gfx {
namespace {

constexpr auto to_d3d_color_value(const Color& color) noexcept
  -> D3DCOLORVALUE {
  return D3DCOLORVALUE {color.red(), color.green(), color.blue(),
                        color.alpha()};
}

constexpr auto to_d3d(const Mat4f32& mat) noexcept -> D3DMATRIX {
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

// TODO: needs some form of validation
auto to_fvf(const VertexLayout& layout) -> DWORD {
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

struct D3D9RenderState {
  D3DRENDERSTATETYPE state;
  DWORD value;
};

// the RenderState visitor below doesn't find the Color overload without this
// with the MSVC compiler.
// TODO: is it a compiler bug?
using gfx::to_d3d;

auto to_d3d(const RenderState& rs) -> D3D9RenderState {
  static constexpr EnumArray<RenderStateType, D3DRENDERSTATETYPE, 2> TO_D3D {
    {RenderStateType::FillMode, D3DRS_FILLMODE},
    {RenderStateType::ShadeMode, D3DRS_SHADEMODE},
  };

  static_assert(RENDER_STATE_COUNT == TO_D3D.size());

  return D3D9RenderState {
    TO_D3D[rs.type()],
    std::visit([](auto&& value) -> DWORD { return to_d3d(value); }, rs.value()),
  };
}

auto to_d3d(const TextureFilter filter) -> D3DTEXTUREFILTERTYPE {
  static constexpr EnumArray<TextureFilter, D3DTEXTUREFILTERTYPE, 3> TO_D3D {
    {TextureFilter::Point, D3DTEXF_POINT},
    {TextureFilter::Linear, D3DTEXF_LINEAR},
    {TextureFilter::LinearAnisotropic, D3DTEXF_ANISOTROPIC},
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
    {TextureAddressMode::WrapRepeat, D3DTADDRESS_WRAP},
    {TextureAddressMode::MirrorRepeat, D3DTADDRESS_MIRROR},
    {TextureAddressMode::ClampEdge, D3DTADDRESS_CLAMP},
  };

  static_assert(TEXTURE_ADDRESS_MODE_COUNT == TO_D3D.size());

  return TO_D3D[mode];
}

struct D3D9TextureStageState {
  D3DTEXTURESTAGESTATETYPE state;
  DWORD value;
};

auto to_d3d_texture_stage_state(const TextureStageState state, const u32 value)
  -> D3D9TextureStageState {
  static constexpr std::array<D3DTEXTURESTAGESTATETYPE, 2>
    TEXTURE_STAGE_STATE_CONV = {
      /* CoordinateSource      */ D3DTSS_TEXCOORDINDEX,
      /* TextureTransformFlags */ D3DTSS_TEXTURETRANSFORMFLAGS,
    };
  static constexpr EnumArray<TextureStageState, D3DTEXTURESTAGESTATETYPE, 2>
    TO_D3D {
      {TextureStageState::CoordinateSource, D3DTSS_TEXCOORDINDEX},
      {TextureStageState::TextureTransformFlags, D3DTSS_TEXTURETRANSFORMFLAGS},
    };
  static_assert(TEXTURE_STAGE_STATE_COUNT == TEXTURE_STAGE_STATE_CONV.size());

  DWORD d3dValue {0};

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

  return D3D9TextureStageState {TO_D3D[state], d3dValue};
}

auto map_vertex_buffer(IDirect3DVertexBuffer9& vertexBuffer,
                       const uDeviceSize offset, uDeviceSize size)
  -> gsl::span<std::byte> {
  D3DVERTEXBUFFER_DESC desc {};
  D3D9CALL(vertexBuffer.GetDesc(&desc));

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

  static void execute(const ext::CommandRenderDearImGui&) {
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
  explicit D3D9XModelSupport(ComPtr<IDirect3DDevice9> device)
    : mDevice {std::move(device)} {
  }

  void execute(const ext::CommandDrawXModel& cmd) const {
    const auto& model = mModels[cmd.handle];
    for (DWORD i = 0; i < model.materials.size(); i++) {
      D3D9CALL(mDevice->SetMaterial(&model.materials[i]));
      D3D9CALL(mDevice->SetTexture(0, model.textures[i].Get()));

      model.mesh->DrawSubset(i);
    }

    D3D9CALL(mDevice->SetTexture(0, nullptr));
  }

  auto load(string_view filePath) -> ext::XModel override {
    const auto [handle, model] = mModels.allocate();

    const auto wideFilePath = create_wide_from_utf8(filePath);

    ComPtr<ID3DXBuffer> materialBuffer;
    DWORD numMaterials {};
    auto hr = ::D3DXLoadMeshFromXW(wideFilePath.c_str(), D3DXMESH_MANAGED,
                                   mDevice.Get(), nullptr,
                                   materialBuffer.GetAddressOf(), nullptr,
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

  D3D9CALL(mDevice->GetDeviceCaps(&mD3D9Caps));

  mCaps.maxVertexBufferSizeInBytes = std::numeric_limits<UINT>::max();
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

// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void D3D9Device::execute(const CommandList& cmdList) {
  // set device state to what the command list expects as default
  // TODO: use state block
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"set default state");

  mCurrentPrimitiveType = D3DPT_POINTLIST;

  D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));

  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));

  constexpr D3DMATRIX identity {to_d3d(Mat4f32::identity())};
  D3D9CALL(mDevice->SetTransform(D3DTS_PROJECTION, &identity));
  D3D9CALL(mDevice->SetTransform(D3DTS_VIEW, &identity));
  D3D9CALL(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &identity));

  D3D9CALL(mDevice->SetRenderState(D3DRS_AMBIENT, 0u));

  PIX_END_EVENT();

  // overload resolution fails if one of the execute overloads is const
  // TODO: MSVC compiler bug?

  auto visitor {
    [&](auto&& cmd) { this->execute(std::forward<decltype(cmd)>(cmd)); }};

  std::for_each(cmdList.begin(), cmdList.end(),
                [&](const Command* cmd) { visit(*cmd, visitor); });

  // disable used lights
  for (u8 i = 0; i < mMaxLightsUsed; i++) {
    D3D9CALL(mDevice->LightEnable(i, FALSE));
  }

  // reset render states
  // TODO: use state block
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP));
  D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
  D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                         D3DTTFF_DISABLE));
  D3D9CALL(mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  D3D9CALL(mDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD));

  D3D9CALL(mDevice->SetTransform(D3DTS_TEXTURE0, &identity));

  D3D9CALL(mDevice->SetTexture(0, nullptr));
  // D3D9CALL(mDevice->SetFVF(0));

  // unbind resources
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"unbind resources");
  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
  PIX_END_EVENT();
}

void D3D9Device::end_execution() const {
  D3D9CALL(mDevice->EndScene());
}

auto D3D9Device::create_pipeline(const PipelineDescriptor& desc) -> Pipeline {
  return std::get<0>(mPipelines.allocate(PipelineData {
    to_d3d(desc.primitiveType),
    to_d3d(desc.lighting),
    to_d3d(desc.cullMode),
    desc.depthTest == DepthTestPass::Always && !desc.depthWriteEnable
      ? D3DZB_FALSE
      : D3DZB_TRUE,
    to_d3d(desc.depthTest),
    to_d3d(desc.depthWriteEnable),
  }));
}

void D3D9Device::destroy_pipeline(const Pipeline handle) noexcept {
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

  D3D9VertexBuffer vertexBuffer {};
  if (FAILED(mDevice->CreateVertexBuffer(size, 0ul, fvf, D3DPOOL_MANAGED,
                                         &vertexBuffer, nullptr))) {
    BASALT_LOG_ERROR("failed to allocate vertex buffer");

    throw bad_alloc {};
  }

  if (!initialData.empty()) {
    // TODO: should initialData.size() > size be an error?
    // TODO: should failing to upload initial data be an error?
    if (const gsl::span vertexBufferData {
          gfx::map_vertex_buffer(*vertexBuffer.Get(), 0, 0)};
        !vertexBufferData.empty()) {
      std::copy_n(initialData.begin(),
                  std::min(initialData.size(), uSize {size}),
                  vertexBufferData.begin());

      D3D9CALL(vertexBuffer->Unlock());
    }
  }

  return std::get<0>(mVertexBuffers.allocate(std::move(vertexBuffer)));
}

void D3D9Device::destroy_vertex_buffer(const VertexBuffer handle) noexcept {
  mVertexBuffers.deallocate(handle);
}

auto D3D9Device::map_vertex_buffer(const VertexBuffer handle,
                                   const uDeviceSize offset,
                                   const uDeviceSize size)
  -> gsl::span<std::byte> {
  BASALT_ASSERT(mVertexBuffers.is_handle_valid(handle));

  if (!mVertexBuffers.is_handle_valid(handle)) {
    return {};
  }

  const D3D9VertexBuffer& vertexBuffer {mVertexBuffers[handle]};

  return gfx::map_vertex_buffer(*vertexBuffer.Get(), offset, size);
}

void D3D9Device::unmap_vertex_buffer(const VertexBuffer handle) noexcept {
  BASALT_ASSERT(mVertexBuffers.is_handle_valid(handle));

  if (!mVertexBuffers.is_handle_valid(handle)) {
    return;
  }

  const D3D9VertexBuffer& vertexBuffer {mVertexBuffers[handle]};

  D3D9CALL(vertexBuffer->Unlock());
}

auto D3D9Device::load_texture(const path& filePath) -> Texture {
  const auto [handle, texture] = mTextures.allocate();

  if (FAILED(D3DXCreateTextureFromFileExW(
        mDevice.Get(), filePath.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, nullptr, nullptr, &texture))) {
    throw std::runtime_error {"loading texture file failed"};
  }

  return handle;
}

auto D3D9Device::create_sampler(const SamplerDescriptor& desc) -> Sampler {
  const auto [handle, data] = mSamplers.allocate();

  data.filter = to_d3d(desc.filter);
  data.mipFilter = to_d3d(desc.mipFilter);
  data.addressModeU = to_d3d(desc.addressModeU);
  data.addressModeV = to_d3d(desc.addressModeV);
  data.addressModeW = to_d3d(desc.addressModeW);

  return handle;
}

auto D3D9Device::query_extension(const ext::ExtensionId id)
  -> optional<ext::ExtensionPtr> {
  if (const auto entry = mExtensions.find(id); entry != mExtensions.end()) {
    return entry->second;
  }

  return std::nullopt;
}

void D3D9Device::execute(const Command& cmd) {
  switch (cmd.type) {
  case CommandType::ExtDrawXModel:
    std::static_pointer_cast<const D3D9XModelSupport>(
      mExtensions[ext::ExtensionId::XModelSupport])
      ->execute(cmd.as<ext::CommandDrawXModel>());
    break;

  case CommandType::ExtRenderDearImGui:
    D3D9ImGuiRenderer::execute(cmd.as<ext::CommandRenderDearImGui>());
    break;

  default:
    BASALT_LOG_ERROR("d3d9 device can't handle this command");
  }
}

void D3D9Device::execute(const CommandClearAttachments& cmd) {
  const DWORD flags {[&] {
    DWORD f {0};

    if (cmd.attachments.has(Attachment::Color)) {
      f |= D3DCLEAR_TARGET;
    }

    if (cmd.attachments.has(Attachment::ZBuffer)) {
      f |= D3DCLEAR_ZBUFFER;
    }

    if (cmd.attachments.has(Attachment::StencilBuffer)) {
      f |= D3DCLEAR_STENCIL;
    }

    return f;
  }()};

  D3D9CALL(
    mDevice->Clear(0u, nullptr, flags, to_d3d(cmd.color), cmd.z, cmd.stencil));
}

void D3D9Device::execute(const CommandDraw& cmd) {
  D3D9CALL(mDevice->DrawPrimitive(
    mCurrentPrimitiveType, cmd.firstVertex,
    calculate_primitive_count(mCurrentPrimitiveType, cmd.vertexCount)));
}

void D3D9Device::execute(const CommandSetRenderState& cmd) {
  const auto [state, value] {to_d3d(cmd.renderState)};

  D3D9CALL(mDevice->SetRenderState(state, value));
}

void D3D9Device::execute(const CommandBindPipeline& cmd) {
  BASALT_ASSERT(mPipelines.is_handle_valid(cmd.handle));

  if (!mPipelines.is_handle_valid(cmd.handle)) {
    return;
  }

  const PipelineData& data {mPipelines[cmd.handle]};
  mCurrentPrimitiveType = data.primitiveType;

  D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, data.lighting));
  D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, data.cullMode));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZENABLE, data.zEnabled));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZFUNC, data.zFunc));
  D3D9CALL(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, data.zWriteEnabled));
}

void D3D9Device::execute(const CommandBindVertexBuffer& cmd) {
  BASALT_ASSERT(mVertexBuffers.is_handle_valid(cmd.handle));

  if (!mVertexBuffers.is_handle_valid(cmd.handle)) {
    return;
  }

  const D3D9VertexBuffer& buffer {mVertexBuffers[cmd.handle]};

  D3DVERTEXBUFFER_DESC desc {};
  D3D9CALL(buffer->GetDesc(&desc));

  D3D9CALL(mDevice->SetFVF(desc.FVF));

  const UINT fvfStride {D3DXGetFVFVertexSize(desc.FVF)};
  const UINT maxOffset {desc.Size - fvfStride};
  BASALT_ASSERT(cmd.offset < maxOffset);

  const UINT offset {std::min(maxOffset, static_cast<UINT>(cmd.offset))};

  D3D9CALL(mDevice->SetStreamSource(0u, buffer.Get(), offset, fvfStride));
}

void D3D9Device::execute(const CommandBindSampler& cmd) {
  const auto& data {mSamplers[cmd.sampler]};

  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, data.filter));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, data.filter));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, data.mipFilter));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY,
                                    mD3D9Caps.MaxAnisotropy));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, data.addressModeU));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, data.addressModeV));
  D3D9CALL(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, data.addressModeW));
}

void D3D9Device::execute(const CommandBindTexture& cmd) {
  const auto& texture = mTextures[cmd.texture];
  D3D9CALL(mDevice->SetTexture(0, texture.Get()));
}

void D3D9Device::execute(const CommandSetTransform& cmd) {
  BASALT_ASSERT_IF(cmd.state == TransformState::ViewToViewport,
                   cmd.transform.m34 >= 0,
                   "(3,4) can't be negative in a projection matrix");

  const D3DTRANSFORMSTATETYPE state {to_d3d(cmd.state)};
  const auto transform {to_d3d(cmd.transform)};

  D3D9CALL(mDevice->SetTransform(state, &transform));
}

void D3D9Device::execute(const CommandSetAmbientLight& cmd) {
  D3D9CALL(mDevice->SetRenderState(D3DRS_AMBIENT, to_d3d(cmd.ambientColor)));
}

void D3D9Device::execute(const CommandSetLights& cmd) {
  const auto& directionalLights {cmd.lights};
  BASALT_ASSERT(directionalLights.size() <= mD3D9Caps.MaxActiveLights,
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

void D3D9Device::execute(const CommandSetMaterial& cmd) {
  const D3DMATERIAL9 material {
    to_d3d_color_value(cmd.diffuse),
    to_d3d_color_value(cmd.ambient),
    D3DCOLORVALUE {},
    to_d3d_color_value(cmd.emissive),
    0.0f,
  };

  D3D9CALL(mDevice->SetMaterial(&material));
}

void D3D9Device::execute(const CommandSetTextureStageState& cmd) {
  const auto [state, value] {to_d3d_texture_stage_state(cmd.state, cmd.value)};

  D3D9CALL(mDevice->SetTextureStageState(0, state, value));
}

} // namespace basalt::gfx
