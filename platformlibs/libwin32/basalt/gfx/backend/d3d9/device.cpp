#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/d3d9/conversions.h>
#include <basalt/gfx/backend/d3d9/dear_imgui_renderer.h>
#include <basalt/gfx/backend/d3d9/x_model_support.h>

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/utils.h>

#include <gsl/span>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>
#include <variant>

using namespace std::literals;

using std::bad_alloc;
using std::numeric_limits;
using std::optional;
using std::filesystem::path;

using gsl::span;

using Microsoft::WRL::ComPtr;

namespace basalt::gfx {
namespace {

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

auto map_impl(IDirect3DVertexBuffer9& vertexBuffer,
              const uDeviceSize offset = 0, uDeviceSize size = 0)
  -> span<std::byte> {
  D3DVERTEXBUFFER_DESC desc {};
  D3D9CHECK(vertexBuffer.GetDesc(&desc));

  if (size == 0) {
    size = desc.Size - offset;
  }

  if (offset >= desc.Size || size + offset > desc.Size) {
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

  if (sizeInBytes == 0) {
    sizeInBytes = desc.Size - offsetInBytes;
  }

  if (offsetInBytes >= desc.Size || sizeInBytes + offsetInBytes > desc.Size) {
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

} // namespace

D3D9Device::D3D9Device(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
  BASALT_ASSERT(mDevice);

  mExtensions[ext::ExtensionId::DearImGuiRenderer] =
    ext::D3D9ImGuiRenderer::create(mDevice);

  mExtensions[ext::ExtensionId::XModelSupport] =
    ext::D3D9XModelSupport::create(mDevice);

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
  const ext::D3D9ImGuiRendererPtr imguiRenderer {
    get_extension<ext::D3D9ImGuiRenderer>()};

  imguiRenderer->invalidate_device_objects();

  // TODO: test cooperative level (see D3D9Context::present)
  D3D9CHECK(mDevice->Reset(&pp));

  imguiRenderer->create_device_objects();
}

// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
auto D3D9Device::execute(const CommandList& cmdList) -> void {
  // overload resolution fails if one of the execute overloads is const
  // TODO: MSVC compiler bug?

  auto visitor {
    [&](auto&& cmd) { this->execute(std::forward<decltype(cmd)>(cmd)); }};

  for (const auto* cmd : cmdList) {
    visit(*cmd, visitor);
  }
}

auto D3D9Device::capabilities() const -> const DeviceCaps& {
  return mCaps;
}

auto D3D9Device::create_pipeline(const PipelineDescriptor& desc) -> Pipeline {
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

  // TODO: is there a benefit to turn off z testing when func = Always
  // and with writing disabled?
  const D3DZBUFFERTYPE zEnabled {desc.depthTest == TestPassCond::Always &&
                                     !desc.depthWriteEnable
                                   ? D3DZB_FALSE
                                   : D3DZB_TRUE};

  auto toVertexFogMode {
    [](const FogMode mode, const FogType type) -> D3DFOGMODE {
      if (type == FogType::None || type == FogType::Fragment) {
        return D3DFOG_NONE;
      }

      return to_d3d(mode);
    }};

  auto toTableFogMode {
    [](const FogMode mode, const FogType type) -> D3DFOGMODE {
      if (type == FogType::Fragment) {
        return to_d3d(mode);
      }

      return D3DFOG_NONE;
    }};

  const bool alphaTestEnabled {desc.alphaTest != TestPassCond::Always};
  const bool alphaBlendEnabled {desc.blendOp != BlendOp::Add ||
                                desc.srcBlendFactor != BlendFactor::One ||
                                desc.destBlendFactor != BlendFactor::Zero};

  // TODO: split texture stage args into color and alpha args
  return mPipelines.allocate(PipelineData {
    to_fvf(desc.vertexInputState),
    stage1Tci,
    stage0Ttf,
    stage1Arg1,
    stage1Arg2,
    stage1ColorOp,
    stage1AlphaOp,
    to_d3d(desc.primitiveType),
    desc.lightingEnabled,
    to_d3d(desc.shadeMode),
    to_d3d(desc.cullMode),
    to_d3d(desc.fillMode),
    zEnabled,
    to_d3d(desc.depthTest),
    desc.depthWriteEnable,
    desc.dithering,
    desc.fogType != FogType::None,
    toVertexFogMode(desc.fogMode, desc.fogType),
    desc.fogType == FogType::VertexRangeBased,
    toTableFogMode(desc.fogMode, desc.fogType),
    desc.vertexColorEnabled,
    to_d3d(desc.diffuseSource),
    to_d3d(desc.specularSource),
    to_d3d(desc.ambientSource),
    to_d3d(desc.emissiveSource),
    desc.specularEnabled,
    desc.normalizeViewSpaceNormals,
    alphaTestEnabled,
    to_d3d(desc.alphaTest),
    alphaBlendEnabled,
    to_d3d(desc.srcBlendFactor),
    to_d3d(desc.destBlendFactor),
    to_d3d(desc.blendOp),
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
  if (desc.sizeInBytes > mCaps.maxVertexBufferSizeInBytes) {
    throw bad_alloc {};
  }

  const DWORD fvf {to_fvf(desc.layout)};
  const UINT size {static_cast<UINT>(desc.sizeInBytes)};

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
  if (!mVertexBuffers.is_valid(handle)) {
    return {};
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  return map_impl(*vertexBuffer.Get(), offset, size);
}

auto D3D9Device::unmap(const VertexBuffer handle) noexcept -> void {
  if (!mVertexBuffers.is_valid(handle)) {
    return;
  }

  const D3D9VertexBufferPtr& vertexBuffer {mVertexBuffers[handle]};

  D3D9CHECK(vertexBuffer->Unlock());
}

auto D3D9Device::create_index_buffer(const IndexBufferDescriptor& desc,
                                     const span<const std::byte> initialData)
  -> IndexBuffer {
  if (desc.sizeInBytes > mCaps.maxIndexBufferSizeInBytes) {
    throw bad_alloc {};
  }

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
  if (!mIndexBuffers.is_valid(handle)) {
    return {};
  }

  const D3D9IndexBufferPtr& indexBuffer {mIndexBuffers[handle]};

  return map_impl(*indexBuffer.Get(), offsetInBytes, sizeInBytes);
}

auto D3D9Device::unmap(const IndexBuffer handle) noexcept -> void {
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

auto D3D9Device::submit(const span<CommandList> commandLists) -> void {
  // TODO: should we make all rendering code dependent
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CHECK(mDevice->BeginScene());

  for (const auto& commandList : commandLists) {
    PIX_BEGIN_EVENT(0, L"command list");

    execute(commandList);

    PIX_END_EVENT();
  }

  // unbind resources
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"unbind resources");
  D3D9CHECK(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
  D3D9CHECK(mDevice->SetIndices(nullptr));
  D3D9CHECK(mDevice->SetTexture(0, nullptr));
  PIX_END_EVENT();

  D3D9CHECK(mDevice->EndScene());
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
    get_extension<const ext::D3D9XModelSupport>()->execute(
      cmd.as<ext::CommandDrawXMesh>());
    break;

  case CommandType::ExtRenderDearImGui:
    get_extension<const ext::D3D9ImGuiRenderer>()->execute(
      cmd.as<ext::CommandRenderDearImGui>());
    break;

  default:
    BASALT_LOG_ERROR("d3d9 device can't handle this command");
  }
}

auto D3D9Device::execute(const CommandClearAttachments& cmd) -> void {
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

  D3D9CHECK(mDevice->Clear(0u, nullptr, flags, to_d3d_color(cmd.color),
                           cmd.depth, cmd.stencil));
}

auto D3D9Device::execute(const CommandDraw& cmd) -> void {
  const UINT primitiveCount {
    calculate_primitive_count(mCurrentPrimitiveType, cmd.vertexCount)};

  D3D9CHECK(mDevice->DrawPrimitive(mCurrentPrimitiveType, cmd.firstVertex,
                                   primitiveCount));
}

auto D3D9Device::execute(const CommandDrawIndexed& cmd) -> void {
  const UINT primitiveCount {
    calculate_primitive_count(mCurrentPrimitiveType, cmd.indexCount)};

  D3D9CHECK(mDevice->DrawIndexedPrimitive(
    mCurrentPrimitiveType, cmd.vertexOffset, cmd.minIndex, cmd.numVertices,
    cmd.firstIndex, primitiveCount));
}

auto D3D9Device::execute(const CommandBindPipeline& cmd) -> void {
  if (!mPipelines.is_valid(cmd.pipelineId)) {
    return;
  }

  PIX_BEGIN_EVENT(0, L"CommandBindPipeline");

  const PipelineData& data {mPipelines[cmd.pipelineId]};
  mCurrentPrimitiveType = data.primitiveType;

  D3D9CHECK(mDevice->SetFVF(data.fvf));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_LIGHTING, data.lightingEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_SPECULARENABLE, data.specularEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_COLORVERTEX, data.vertexColorEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, data.diffuseSource));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, data.specularSource));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, data.ambientSource));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, data.emissiveSource));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_NORMALIZENORMALS,
                                    data.normalizeViewSpaceNormals));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_SHADEMODE, data.shadeMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CULLMODE, data.cullMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FILLMODE, data.fillMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZENABLE, data.zEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZFUNC, data.zFunc));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, data.zWriteEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_DITHERENABLE, data.dithering));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGENABLE, data.fogEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGVERTEXMODE, data.vertexFogMode));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, data.alphaTestEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ALPHAFUNC, data.alphaFunc));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, data.alphaBlendEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_SRCBLEND, data.srcBlend));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_DESTBLEND, data.destBlend));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_BLENDOP, data.blendOp));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_RANGEFOGENABLE, data.vertexFogRanged));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGTABLEMODE, data.tableFogMode));

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

  PIX_END_EVENT();
}

auto D3D9Device::execute(const CommandBindVertexBuffer& cmd) -> void {
  if (!mVertexBuffers.is_valid(cmd.vertexBufferId)) {
    return;
  }

  const D3D9VertexBufferPtr& buffer {mVertexBuffers[cmd.vertexBufferId]};

  D3DVERTEXBUFFER_DESC desc {};
  D3D9CHECK(buffer->GetDesc(&desc));

  const UINT fvfStride {D3DXGetFVFVertexSize(desc.FVF)};
  const UINT offset {static_cast<UINT>(cmd.offsetInBytes)};

  D3D9CHECK(mDevice->SetStreamSource(0u, buffer.Get(), offset, fvfStride));
}

auto D3D9Device::execute(const CommandBindIndexBuffer& cmd) -> void {
  if (!mIndexBuffers.is_valid(cmd.indexBufferId)) {
    return;
  }

  const D3D9IndexBufferPtr& buffer {mIndexBuffers[cmd.indexBufferId]};

  D3D9CHECK(mDevice->SetIndices(buffer.Get()));
}

auto D3D9Device::execute(const CommandBindSampler& cmd) -> void {
  if (!mSamplers.is_valid(cmd.samplerId)) {
    return;
  }

  const auto& data {mSamplers[cmd.samplerId]};

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
  if (!mTextures.is_valid(cmd.textureId)) {
    return;
  }

  const D3D9TexturePtr& texture {mTextures[cmd.textureId]};

  D3D9CHECK(mDevice->SetTexture(0, texture.Get()));
}

auto D3D9Device::execute(const CommandSetBlendConstant& cmd) -> void {
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_BLENDFACTOR, to_d3d_color(cmd.value)));
}

auto D3D9Device::execute(const CommandSetTransform& cmd) -> void {
  const D3DTRANSFORMSTATETYPE state {to_d3d(cmd.transformState)};
  const auto transform {to_d3d(cmd.transform)};

  D3D9CHECK(mDevice->SetTransform(state, &transform));
}

auto D3D9Device::execute(const CommandSetAmbientLight& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, to_d3d_color(cmd.ambient)));
}

auto D3D9Device::execute(const CommandSetLights& cmd) -> void {
  PIX_BEGIN_EVENT(0, L"CommandSetLights");

  const auto& lights {cmd.lights};
  const DWORD numLights {
    std::min(saturated_cast<u32>(lights.size()), mCaps.maxLights)};

  DWORD lightIndex {0ul};
  for (; lightIndex < numLights; lightIndex++) {
    D3DLIGHT9 d3dLight {
      visit([](auto&& light) { return to_d3d(light); }, lights[lightIndex])};

    D3D9CHECK(mDevice->SetLight(lightIndex, &d3dLight));
    D3D9CHECK(mDevice->LightEnable(lightIndex, TRUE));
  }

  // disable previously enabled lights
  for (; lightIndex < mNumLightsUsed; lightIndex++) {
    D3D9CHECK(mDevice->LightEnable(lightIndex, FALSE));
  }

  mNumLightsUsed = numLights;

  PIX_END_EVENT();
}

auto D3D9Device::execute(const CommandSetMaterial& cmd) -> void {
  const D3DMATERIAL9 material {
    to_d3d_color_value(cmd.diffuse),
    to_d3d_color_value(cmd.ambient),
    to_d3d_color_value(cmd.specular),
    to_d3d_color_value(cmd.emissive),
    cmd.specularPower,
  };

  D3D9CHECK(mDevice->SetMaterial(&material));
}

auto D3D9Device::execute(const CommandSetFogParameters& cmd) -> void {
  PIX_BEGIN_EVENT(0, L"CommandSetFogParameters");

  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGCOLOR, to_d3d_color(cmd.color)));
  D3D9CHECK(mDevice->SetRenderState(
    D3DRS_FOGSTART, *reinterpret_cast<const DWORD*>(&cmd.start)));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGEND,
                                    *reinterpret_cast<const DWORD*>(&cmd.end)));
  D3D9CHECK(mDevice->SetRenderState(
    D3DRS_FOGDENSITY, *reinterpret_cast<const DWORD*>(&cmd.density)));

  PIX_END_EVENT();
}

auto D3D9Device::execute(const CommandSetReferenceAlpha& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ALPHAREF, cmd.value));
}

} // namespace basalt::gfx
