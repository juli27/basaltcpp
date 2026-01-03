#include "device.h"

#include "conversions.h"
#include "d3d9_error.h"
#include "dear_imgui_renderer.h"
#include "effect.h"
#include "x_model_support.h"

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>
#include <basalt/api/base/utils.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <variant>

using namespace std::literals;

using std::array;
using std::bad_alloc;
using std::byte;
using std::system_error;
using std::filesystem::path;

using gsl::span;

namespace basalt::gfx {
namespace {

auto calculate_primitive_count(D3DPRIMITIVETYPE const type,
                               u32 const vertexCount) -> u32 {
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

auto D3D9Device::create(IDirect3DDevice9Ptr device, DeviceCaps const& caps)
  -> D3D9DevicePtr {
  return std::make_shared<D3D9Device>(std::move(device), caps);
}

D3D9Device::D3D9Device(IDirect3DDevice9Ptr device, DeviceCaps const& caps)
  : mDevice{std::move(device)}
  , mCaps{caps} {
  BASALT_ASSERT(mDevice);
}

auto D3D9Device::device() const noexcept -> IDirect3DDevice9Ptr const& {
  return mDevice;
}

auto D3D9Device::reset(D3DPRESENT_PARAMETERS& pp) const -> void {
  auto const imguiRenderer = get_extension<ext::D3D9ImGuiRenderer const>();
  auto const effects = get_extension<ext::D3D9XEffects const>();

  imguiRenderer->invalidate_device_objects();
  effects->on_device_lost();

  // TODO: test cooperative level (see D3D9SwapChain::present)
  D3D9CHECK(mDevice->Reset(&pp));

  imguiRenderer->create_device_objects();
  effects->on_device_reset();
}

// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
auto D3D9Device::execute(CommandList const& cmdList) -> void {
  // overload resolution fails if one of the execute overloads is const
  // TODO: MSVC compiler bug?

  auto visitor = [&](auto&& cmd) {
    this->execute(std::forward<decltype(cmd)>(cmd));
  };

  for (auto const* cmd : cmdList) {
    visit(*cmd, visitor);
  }
}

auto D3D9Device::add_texture(IDirect3DBaseTexture9Ptr texture)
  -> TextureHandle {
  return mTextures.emplace(std::move(texture));
}

auto D3D9Device::get_d3d9(TextureHandle const id) const
  -> IDirect3DBaseTexture9Ptr {
  return mTextures[id];
}

auto D3D9Device::set_extensions(ext::DeviceExtensions extensions) -> void {
  mExtensions = std::move(extensions);
}

auto D3D9Device::capabilities() const -> DeviceCaps const& {
  return mCaps;
}

auto D3D9Device::get_status() const noexcept -> DeviceStatus {
  return to_device_status(mDevice->TestCooperativeLevel());
}

auto D3D9Device::reset() -> void {
  auto swapChain = IDirect3DSwapChain9Ptr{};
  D3D9CHECK(mDevice->GetSwapChain(0, &swapChain));
  auto pp = D3DPRESENT_PARAMETERS{};
  D3D9CHECK(swapChain->GetPresentParameters(&pp));
  reset(pp);
}

auto D3D9Device::create_pipeline(PipelineCreateInfo const& desc)
  -> PipelineHandle {
  return mPipelines.emplace(D3D9Pipeline::from(desc));
}

auto D3D9Device::destroy(PipelineHandle const handle) noexcept -> void {
  mPipelines.destroy(handle);
}

// throws std::bad_alloc when requested size is too large and when d3d9
// allocation fails
auto D3D9Device::create_vertex_buffer(VertexBufferCreateInfo const& desc)
  -> VertexBufferHandle {
  if (desc.sizeInBytes > mCaps.maxVertexBufferSizeInBytes) {
    throw bad_alloc{};
  }

  auto const fvf = to_d3d_fvf(desc.layout);
  auto const size = static_cast<UINT>(desc.sizeInBytes);

  auto vertexBuffer = IDirect3DVertexBuffer9Ptr{};
  if (FAILED(mDevice->CreateVertexBuffer(size, 0, fvf, D3DPOOL_MANAGED,
                                         &vertexBuffer, nullptr))) {
    BASALT_LOG_ERROR("failed to allocate vertex buffer");

    throw bad_alloc{};
  }

  return mVertexBuffers.emplace(std::move(vertexBuffer));
}

auto D3D9Device::destroy(VertexBufferHandle const handle) noexcept -> void {
  mVertexBuffers.destroy(handle);
}

auto D3D9Device::map(VertexBufferHandle const handle,
                     uDeviceSize const offsetInBytes, uDeviceSize sizeInBytes)
  -> span<byte> {
  auto const& vertexBuffer = mVertexBuffers[handle];
  auto desc = D3DVERTEXBUFFER_DESC{};
  D3D9CHECK(vertexBuffer->GetDesc(&desc));

  if (sizeInBytes == 0) {
    sizeInBytes = desc.Size - offsetInBytes;
  }

  if (offsetInBytes >= desc.Size || sizeInBytes + offsetInBytes > desc.Size) {
    return {};
  }

  void* vertexBufferData = {};
  if (FAILED(vertexBuffer->Lock(static_cast<UINT>(offsetInBytes),
                                static_cast<UINT>(sizeInBytes),
                                &vertexBufferData, 0))) {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");

    return {};
  }

  return span{static_cast<byte*>(vertexBufferData), sizeInBytes};
}

auto D3D9Device::unmap(VertexBufferHandle const handle) noexcept -> void {
  auto const& vertexBuffer = mVertexBuffers[handle];

  D3D9CHECK(vertexBuffer->Unlock());
}

auto D3D9Device::create_index_buffer(IndexBufferCreateInfo const& desc)
  -> IndexBufferHandle {
  if (desc.sizeInBytes > mCaps.maxIndexBufferSizeInBytes) {
    throw bad_alloc{};
  }

  auto const size = static_cast<UINT>(desc.sizeInBytes);
  auto const type = to_d3d(desc.type);

  auto indexBuffer = IDirect3DIndexBuffer9Ptr{};
  if (FAILED(mDevice->CreateIndexBuffer(size, 0, type, D3DPOOL_MANAGED,
                                        &indexBuffer, nullptr))) {
    BASALT_LOG_ERROR("failed to allocate index buffer");

    throw bad_alloc{};
  }

  return mIndexBuffers.emplace(std::move(indexBuffer));
}

auto D3D9Device::destroy(IndexBufferHandle const handle) noexcept -> void {
  mIndexBuffers.destroy(handle);
}

auto D3D9Device::map(IndexBufferHandle const handle,
                     uDeviceSize const offsetInBytes, uDeviceSize sizeInBytes)
  -> span<byte> {
  auto const& indexBuffer = mIndexBuffers[handle];
  auto desc = D3DINDEXBUFFER_DESC{};
  D3D9CHECK(indexBuffer->GetDesc(&desc));

  if (sizeInBytes == 0) {
    sizeInBytes = desc.Size - offsetInBytes;
  }

  if (offsetInBytes >= desc.Size || sizeInBytes + offsetInBytes > desc.Size) {
    return {};
  }

  void* indexBufferData{};
  if (FAILED(indexBuffer->Lock(static_cast<UINT>(offsetInBytes),
                               static_cast<UINT>(sizeInBytes), &indexBufferData,
                               0))) {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");

    return {};
  }

  return span{static_cast<byte*>(indexBufferData), sizeInBytes};
}

auto D3D9Device::unmap(IndexBufferHandle const handle) noexcept -> void {
  auto const& indexBuffer = mIndexBuffers[handle];

  D3D9CHECK(indexBuffer->Unlock());
}

auto D3D9Device::load_texture(path const& filePath) -> TextureHandle {
  auto texture = IDirect3DTexture9Ptr{};

  if (FAILED(D3DXCreateTextureFromFileExW(
        mDevice.Get(), filePath.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, nullptr, nullptr, &texture))) {
    throw std::runtime_error{"loading texture file failed"};
  }

  return mTextures.emplace(std::move(texture));
}

auto D3D9Device::load_cube_texture(path const& path) -> TextureHandle {
  auto texture = IDirect3DCubeTexture9Ptr{};

  // TODO: Mip map count is fixed to 1
  if (FAILED(D3DXCreateCubeTextureFromFileExW(
        mDevice.Get(), path.c_str(), D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
        &texture))) {
    throw std::runtime_error{"loading texture file failed"};
  }

  return mTextures.emplace(std::move(texture));
}

auto D3D9Device::destroy(TextureHandle const handle) noexcept -> void {
  mTextures.destroy(handle);
}

auto D3D9Device::create_sampler(SamplerCreateInfo const& desc)
  -> SamplerHandle {
  return mSamplers.emplace(SamplerData{
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

auto D3D9Device::destroy(SamplerHandle const handle) noexcept -> void {
  mSamplers.destroy(handle);
}

auto D3D9Device::submit(span<CommandList const> const commandLists) -> void {
  if (auto const hr = mDevice->BeginScene(); FAILED(hr)) {
    throw system_error{hr, d3d9_category(), "failed to begin scene"s};
  }

  for (auto const& commandList : commandLists) {
    PIX_BEGIN_EVENT(0, L"command list");

    execute(commandList);

    PIX_END_EVENT();
  }

  // unbind resources
  PIX_BEGIN_EVENT(D3DCOLOR_XRGB(128, 128, 128), L"unbind resources");
  D3D9CHECK(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));
  D3D9CHECK(mDevice->SetIndices(nullptr));

  for (auto i = DWORD{0}; i < mCaps.maxTextureBlendStages; i++) {
    D3D9CHECK(mDevice->SetTexture(i, nullptr));
  }

  PIX_END_EVENT();

  D3D9CHECK(mDevice->EndScene());
}

auto D3D9Device::execute(Command const& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh:
    get_extension<ext::D3D9XModelSupport const>()->execute(
      cmd.as<ext::CommandDrawXMesh>());
    break;

  case CommandType::ExtRenderDearImGui:
    get_extension<ext::D3D9ImGuiRenderer const>()->execute(
      cmd.as<ext::CommandRenderDearImGui>());
    break;

  case CommandType::ExtBeginEffect:
    get_extension<ext::D3D9XEffects>()->execute(
      cmd.as<ext::CommandBeginEffect>());
    break;

  case CommandType::ExtEndEffect:
    get_extension<ext::D3D9XEffects>()->execute(
      cmd.as<ext::CommandEndEffect>());
    break;

  case CommandType::ExtBeginEffectPass:
    get_extension<ext::D3D9XEffects>()->execute(
      cmd.as<ext::CommandBeginEffectPass>());
    break;

  case CommandType::ExtEndEffectPass:
    get_extension<ext::D3D9XEffects>()->execute(
      cmd.as<ext::CommandEndEffectPass>());
    break;

  default:
    BASALT_CRASH("d3d9 device can't handle this command");
  }
}

auto D3D9Device::execute(CommandClearAttachments const& cmd) -> void {
  auto const flags = [&] {
    auto f = DWORD{0};

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
  }();

  D3D9CHECK(mDevice->Clear(0u, nullptr, flags, to_d3d_color(cmd.color),
                           cmd.depth, cmd.stencil));
}

auto D3D9Device::execute(CommandDraw const& cmd) -> void {
  auto const primitiveCount =
    calculate_primitive_count(mCurrentPrimitiveType, cmd.vertexCount);

  D3D9CHECK(mDevice->DrawPrimitive(mCurrentPrimitiveType, cmd.firstVertex,
                                   primitiveCount));
}

auto D3D9Device::execute(CommandDrawIndexed const& cmd) -> void {
  auto const primitiveCount =
    calculate_primitive_count(mCurrentPrimitiveType, cmd.indexCount);

  D3D9CHECK(mDevice->DrawIndexedPrimitive(
    mCurrentPrimitiveType, cmd.vertexOffset, cmd.minIndex, cmd.numVertices,
    cmd.firstIndex, primitiveCount));
}

auto D3D9Device::execute(CommandBindPipeline const& cmd) -> void {
  PIX_BEGIN_EVENT(0, L"CommandBindPipeline");

  auto const& data = mPipelines[cmd.pipelineId];
  mCurrentPrimitiveType = data.primitiveType;

  D3D9CHECK(mDevice->SetFVF(data.fvf));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_SHADEMODE, data.vs.shadeMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_LIGHTING, data.vs.lightingEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_SPECULARENABLE, data.vs.specularEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_COLORVERTEX, data.vs.vertexColorEnabled));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_NORMALIZENORMALS,
                                    data.vs.normalizeViewSpaceNormals));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,
                                    data.vs.diffuseSource));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,
                                    data.vs.ambientSource));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,
                                    data.vs.emissiveSource));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,
                                    data.vs.specularSource));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGENABLE, data.fog.enabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGVERTEXMODE, data.fog.vertexMode));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_RANGEFOGENABLE, data.fog.vertexRanged));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FOGTABLEMODE, data.fog.tableMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CULLMODE, data.cullMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_FILLMODE, data.fillMode));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZENABLE, data.zEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZFUNC, data.zFunc));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ZWRITEENABLE, data.zWriteEnabled));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILENABLE, data.stencilEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_STENCILFUNC, data.cwStencilState.func));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_STENCILFAIL, data.cwStencilState.failOp));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILZFAIL,
                                    data.cwStencilState.passDepthFailOp));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILPASS,
                                    data.cwStencilState.passDepthPassOp));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE,
                                    data.twoSidedStencilEnabled));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, data.ccwStencilState.func));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CCW_STENCILFAIL,
                                    data.ccwStencilState.failOp));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL,
                                    data.ccwStencilState.passDepthFailOp));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_CCW_STENCILPASS,
                                    data.ccwStencilState.passDepthPassOp));

  D3D9CHECK(mDevice->SetRenderState(D3DRS_DITHERENABLE, data.dithering));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, data.alphaTestEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ALPHAFUNC, data.alphaFunc));
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, data.alphaBlendEnabled));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_SRCBLEND, data.srcBlend));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_DESTBLEND, data.destBlend));
  D3D9CHECK(mDevice->SetRenderState(D3DRS_BLENDOP, data.blendOp));

  auto stageId = DWORD{0};
  for (auto const& stage : data.textureStages) {
    D3D9CHECK(
      mDevice->SetTextureStageState(stageId, D3DTSS_COLOROP, stage.colorOp));

    if (stage.colorOp == D3DTOP_DISABLE) {
      // the first disabled color op disables all following stages
      break;
    }

    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_COLORARG1,
                                            stage.colorArg1));
    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_COLORARG2,
                                            stage.colorArg2));
    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_COLORARG0,
                                            stage.colorArg3));

    D3D9CHECK(
      mDevice->SetTextureStageState(stageId, D3DTSS_ALPHAOP, stage.alphaOp));
    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_ALPHAARG1,
                                            stage.alphaArg1));
    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_ALPHAARG2,
                                            stage.alphaArg2));
    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_ALPHAARG0,
                                            stage.alphaArg3));

    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_RESULTARG,
                                            stage.resultArg));

    D3D9CHECK(mDevice->SetTextureStageState(stageId, D3DTSS_TEXCOORDINDEX,
                                            stage.coordinateIndex));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_TEXTURETRANSFORMFLAGS, stage.coordinateTransformFlags));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVMAT00,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvMat00)));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVMAT01,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvMat01)));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVMAT10,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvMat10)));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVMAT11,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvMat11)));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVLSCALE,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvLScale)));
    D3D9CHECK(mDevice->SetTextureStageState(
      stageId, D3DTSS_BUMPENVLOFFSET,
      *reinterpret_cast<const DWORD*>(&stage.bumpEnvLOffset)));

    stageId++;
  }

  PIX_END_EVENT();
}

auto D3D9Device::execute(CommandBindVertexBuffer const& cmd) -> void {
  auto const& buffer = mVertexBuffers[cmd.vertexBufferId];

  auto desc = D3DVERTEXBUFFER_DESC{};
  D3D9CHECK(buffer->GetDesc(&desc));

  auto const fvfStride = D3DXGetFVFVertexSize(desc.FVF);
  auto const offset = static_cast<UINT>(cmd.offsetInBytes);

  D3D9CHECK(mDevice->SetStreamSource(0u, buffer.Get(), offset, fvfStride));
}

auto D3D9Device::execute(CommandBindIndexBuffer const& cmd) -> void {
  auto const& buffer = mIndexBuffers[cmd.indexBufferId];

  D3D9CHECK(mDevice->SetIndices(buffer.Get()));
}

auto D3D9Device::execute(CommandBindSampler const& cmd) -> void {
  PIX_BEGIN_EVENT(0, L"CommandBindSampler");

  auto const& data = mSamplers[cmd.samplerId];

  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_MINFILTER, data.minFilter));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_MAGFILTER, data.magFilter));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_MIPFILTER, data.mipFilter));
  D3D9CHECK(mDevice->SetSamplerState(cmd.slot, D3DSAMP_MAXANISOTROPY,
                                     data.maxAnisotropy));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_ADDRESSU, data.addressModeU));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_ADDRESSV, data.addressModeV));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_ADDRESSW, data.addressModeW));
  D3D9CHECK(
    mDevice->SetSamplerState(cmd.slot, D3DSAMP_BORDERCOLOR, data.borderColor));

  PIX_END_EVENT();
}

auto D3D9Device::execute(CommandBindTexture const& cmd) -> void {
  auto const& texture = mTextures[cmd.textureId];

  D3D9CHECK(mDevice->SetTexture(cmd.slot, texture.Get()));
}

auto D3D9Device::execute(CommandSetStencilReference const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILREF, cmd.value));
}

auto D3D9Device::execute(CommandSetStencilReadMask const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILMASK, cmd.value));
}

auto D3D9Device::execute(CommandSetStencilWriteMask const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_STENCILWRITEMASK, cmd.value));
}

auto D3D9Device::execute(CommandSetBlendConstant const& cmd) -> void {
  D3D9CHECK(
    mDevice->SetRenderState(D3DRS_BLENDFACTOR, to_d3d_color(cmd.value)));
}

auto D3D9Device::execute(CommandSetTransform const& cmd) -> void {
  auto const state = to_d3d(cmd.transformState);
  auto const transform{to_d3d(cmd.transform)};

  D3D9CHECK(mDevice->SetTransform(state, &transform));
}

auto D3D9Device::execute(CommandSetAmbientLight const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_AMBIENT, to_d3d_color(cmd.ambient)));
}

auto D3D9Device::execute(CommandSetLights const& cmd) -> void {
  PIX_BEGIN_EVENT(0, L"CommandSetLights");

  auto const& lights = cmd.lights;
  auto const numLights =
    std::min(saturated_cast<u32>(lights.size()), mCaps.maxLights);

  auto lightIndex = DWORD{0};
  for (; lightIndex < numLights; lightIndex++) {
    auto const d3dLight =
      visit([](auto&& light) { return to_d3d(light); }, lights[lightIndex]);

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

auto D3D9Device::execute(CommandSetMaterial const& cmd) -> void {
  auto const material = D3DMATERIAL9{
    to_d3d_color_value(cmd.diffuse),
    to_d3d_color_value(cmd.ambient),
    to_d3d_color_value(cmd.specular),
    to_d3d_color_value(cmd.emissive),
    cmd.specularPower,
  };

  D3D9CHECK(mDevice->SetMaterial(&material));
}

auto D3D9Device::execute(CommandSetFogParameters const& cmd) -> void {
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

auto D3D9Device::execute(CommandSetReferenceAlpha const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_ALPHAREF, cmd.value));
}

auto D3D9Device::execute(CommandSetTextureFactor const& cmd) -> void {
  D3D9CHECK(mDevice->SetRenderState(D3DRS_TEXTUREFACTOR,
                                    to_d3d_color(cmd.textureFactor)));
}

auto D3D9Device::execute(CommandSetTextureStageConstant const& cmd) -> void {
  D3D9CHECK(mDevice->SetTextureStageState(cmd.stageId, D3DTSS_CONSTANT,
                                          to_d3d_color(cmd.constant)));
}

template <typename T>
auto D3D9Device::get_extension() const -> std::shared_ptr<T> {
  return std::static_pointer_cast<T>(mExtensions.at(T::ID));
}

} // namespace basalt::gfx
