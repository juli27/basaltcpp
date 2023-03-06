#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/d3d9/conversions.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/utils.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/utils.h>

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
using std::numeric_limits;
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

auto map_impl(IDirect3DVertexBuffer9& vertexBuffer,
              const uDeviceSize offset = 0, uDeviceSize size = 0)
  -> span<std::byte> {
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
  if (FAILED(vertexBuffer.Lock(static_cast<UINT>(offset),
                               static_cast<UINT>(size), &vertexBufferData,
                               0ul))) {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");

    return {};
  }

  return {static_cast<std::byte*>(vertexBufferData), size};
}

auto map_impl(IDirect3DIndexBuffer9& indexBuffer,
              const uDeviceSize offsetInBytes = 0, uDeviceSize sizeInBytes = 0)
  -> span<std::byte> {
  D3DINDEXBUFFER_DESC desc {};
  D3D9CHECK(indexBuffer.GetDesc(&desc));

  BASALT_ASSERT(offsetInBytes < desc.Size);
  BASALT_ASSERT(offsetInBytes + sizeInBytes <= desc.Size);

  if (sizeInBytes == 0) {
    sizeInBytes = desc.Size - offsetInBytes;
  }

  if (offsetInBytes >= desc.Size || sizeInBytes + offsetInBytes > desc.Size) {
    BASALT_LOG_ERROR("invalid map params: offsetInBytes = {} sizeInBytes = {} "
                     "(bufferSize = {})",
                     offsetInBytes, sizeInBytes, desc.Size);

    return {};
  }

  void* indexBufferData {};
  if (FAILED(indexBuffer.Lock(static_cast<UINT>(offsetInBytes),
                              static_cast<UINT>(sizeInBytes), &indexBufferData,
                              0ul))) {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");

    return {};
  }

  return {static_cast<std::byte*>(indexBufferData), sizeInBytes};
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
    ImGui_ImplDX9_CreateDeviceObjects();
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

  mCaps.maxVertexBufferSizeInBytes = numeric_limits<UINT>::max();
  mCaps.maxIndexBufferSizeInBytes = numeric_limits<UINT>::max();

  if (d3d9Caps.MaxVertexIndex > 0xffff) {
    mCaps.supportedIndexTypes.set(IndexType::U32);
  }

  mCaps.maxLights = d3d9Caps.MaxActiveLights;
  mCaps.maxTextureBlendStages = d3d9Caps.MaxTextureBlendStages;
  mCaps.maxBoundSampledTextures = d3d9Caps.MaxSimultaneousTextures;

  mCaps.samplerClampToBorder =
    d3d9Caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER;
  mCaps.samplerCustomBorderColor = mCaps.samplerClampToBorder;
  mCaps.samplerMirrorOnceClampToEdge =
    d3d9Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE;

  mCaps.samplerMinFilterAnisotropic =
    d3d9Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC;
  mCaps.samplerMagFilterAnisotropic =
    d3d9Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC;
  mCaps.samplerMaxAnisotropy = saturated_cast<u8>(d3d9Caps.MaxAnisotropy);
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
  // TODO: remove
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"set default state");

  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, 0u));

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
  D3D9CHECK(mDevice->SetIndices(nullptr));
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

  // TODO: is there a benefit to turn off z testing when func = Always
  // and with writing disabled?

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
auto D3D9Device::create_vertex_buffer(const VertexBufferDescriptor& desc,
                                      const span<const std::byte> initialData)
  -> VertexBuffer {
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
    if (const span vertexBufferData {map_impl(*vertexBuffer.Get())};
        !vertexBufferData.empty()) {
      std::copy_n(initialData.begin(),
                  std::min(initialData.size(), uSize {size}),
                  vertexBufferData.begin());

      D3D9CHECK(vertexBuffer->Unlock());
    }
  }

  return mVertexBuffers.allocate(std::move(vertexBuffer));
}

auto D3D9Device::destroy(const VertexBuffer handle) noexcept -> void {
  mVertexBuffers.deallocate(handle);
}

auto D3D9Device::map(const VertexBuffer handle, const uDeviceSize offset,
                     const uDeviceSize size) -> span<std::byte> {
  BASALT_ASSERT(mVertexBuffers.is_valid(handle));

  if (!mVertexBuffers.is_valid(handle)) {
    return {};
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  return map_impl(*vertexBuffer.Get(), offset, size);
}

auto D3D9Device::unmap(const VertexBuffer handle) noexcept -> void {
  BASALT_ASSERT(mVertexBuffers.is_valid(handle));

  if (!mVertexBuffers.is_valid(handle)) {
    return;
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  D3D9CHECK(vertexBuffer->Unlock());
}
auto D3D9Device::create_index_buffer(const IndexBufferDescriptor& desc,
                                     const span<const std::byte> initialData)
  -> IndexBuffer {
  BASALT_ASSERT(initialData.size() <= desc.sizeInBytes);

  if (desc.sizeInBytes > mCaps.maxIndexBufferSizeInBytes) {
    BASALT_LOG_ERROR("requested index buffer size ({}) too large",
                     desc.sizeInBytes);

    throw bad_alloc {};
  }

  BASALT_ASSERT(mCaps.supportedIndexTypes[desc.type]);

  const UINT size {static_cast<UINT>(desc.sizeInBytes)};
  const D3DFORMAT type {to_d3d(desc.type)};

  D3D9IndexBufferPtr indexBuffer {};
  if (FAILED(mDevice->CreateIndexBuffer(size, 0ul, type, D3DPOOL_MANAGED,
                                        &indexBuffer, nullptr))) {
    BASALT_LOG_ERROR("failed to allocate index buffer");

    throw bad_alloc {};
  }

  if (!initialData.empty()) {
    // TODO: should initialData.size() > size be an error?
    // TODO: should failing to upload initial data be an error?
    if (const span indexBufferData {map_impl(*indexBuffer.Get())};
        !indexBufferData.empty()) {
      std::copy_n(initialData.begin(),
                  std::min(initialData.size(), uSize {size}),
                  indexBufferData.begin());

      D3D9CHECK(indexBuffer->Unlock());
    }
  }

  return mIndexBuffers.allocate(std::move(indexBuffer));
}
auto D3D9Device::destroy(const IndexBuffer handle) noexcept -> void {
  mIndexBuffers.deallocate(handle);
}
auto D3D9Device::map(const IndexBuffer handle, const uDeviceSize offsetInBytes,
                     const uDeviceSize sizeInBytes) -> span<std::byte> {
  BASALT_ASSERT(mIndexBuffers.is_valid(handle));

  if (!mIndexBuffers.is_valid(handle)) {
    return {};
  }

  const D3D9IndexBufferPtr& indexBuffer {mIndexBuffers[handle]};

  return map_impl(*indexBuffer.Get(), offsetInBytes, sizeInBytes);
}

auto D3D9Device::unmap(const IndexBuffer handle) noexcept -> void {
  BASALT_ASSERT(mIndexBuffers.is_valid(handle));

  if (!mIndexBuffers.is_valid(handle)) {
    return;
  }

  const D3D9IndexBufferPtr& indexBuffer {mIndexBuffers[handle]};

  D3D9CHECK(indexBuffer->Unlock());
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

auto D3D9Device::destroy(const Texture handle) noexcept -> void {
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
    to_d3d(desc.borderColor, desc.customBorderColor),
    desc.maxAnisotropy,
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
  const UINT primitiveCount {
    calculate_primitive_count(mCurrentPrimitiveType, cmd.vertexCount)};

  D3D9CHECK(mDevice->DrawPrimitive(mCurrentPrimitiveType, cmd.firstVertex,
                                   primitiveCount));
}
auto D3D9Device::execute(const CommandDrawIndexed& cmd) -> void {
  BASALT_ASSERT(mCurrentPrimitiveType != D3DPT_POINTLIST);

  const UINT primitiveCount {
    calculate_primitive_count(mCurrentPrimitiveType, cmd.indexCount)};

  D3D9CHECK(mDevice->DrawIndexedPrimitive(
    mCurrentPrimitiveType, cmd.vertexOffset, cmd.minIndex, cmd.numVertices,
    cmd.firstIndex, primitiveCount));
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
auto D3D9Device::execute(const CommandBindIndexBuffer& cmd) -> void {
  BASALT_ASSERT(mIndexBuffers.is_valid(cmd.handle));

  if (!mIndexBuffers.is_valid(cmd.handle)) {
    return;
  }

  const D3D9IndexBufferPtr& buffer {mIndexBuffers[cmd.handle]};

  D3D9CHECK(mDevice->SetIndices(buffer.Get()));
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
  D3D9CHECK(
    mDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, data.maxAnisotropy));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, data.addressModeU));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, data.addressModeV));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, data.addressModeW));
  D3D9CHECK(mDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, data.borderColor));
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

auto D3D9Device::execute(const CommandSetAmbientLight& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, to_d3d(cmd.ambientColor)));
}

auto D3D9Device::execute(const CommandSetLights& cmd) -> void {
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

auto D3D9Device::execute(const CommandSetMaterial& cmd) -> void {
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
