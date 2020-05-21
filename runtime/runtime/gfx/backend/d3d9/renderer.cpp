#include "runtime/gfx/backend/d3d9/renderer.h"

#include "runtime/gfx/backend/d3d9/util.h"

#include "runtime/math/Mat4.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Color.h"
#include "runtime/shared/Log.h"

#include "runtime/shared/win32/util.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <cstring>
#include <stdexcept>
#include <string_view>
#include <utility>

using Microsoft::WRL::ComPtr;

namespace basalt::gfx::backend {

namespace {

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

constexpr auto to_d3d_color_value(
  const Color& color) noexcept -> D3DCOLORVALUE {
  return {color.red(), color.green(), color.blue(), color.alpha()};
}

constexpr auto to_d3d_matrix(const math::Mat4f32& mat) noexcept -> D3DMATRIX {
  return {
    mat.m11, mat.m12, mat.m13, mat.m14
  , mat.m21, mat.m22, mat.m23, mat.m24
  , mat.m31, mat.m32, mat.m33, mat.m34
  , mat.m41, mat.m42, mat.m43, mat.m44
  };
}

constexpr auto to_d3d_vector(const math::Vec3f32& vec) noexcept -> D3DVECTOR {
  return {vec.x(), vec.y(), vec.z()};
}

auto to_fvf(const VertexLayout& layout) -> DWORD;
auto verify_fvf(DWORD fvf) -> bool;

void fill_primitive_info(
  D3D9Mesh& mesh, PrimitiveType primitiveType, i32 numVtx);

} // namespace

D3D9Renderer::D3D9Renderer(
  ComPtr<IDirect3DDevice9> device, const D3DPRESENT_PARAMETERS& pp)
  : mDevice {std::move(device)}, mPresentParams {pp} {
  BASALT_ASSERT(mDevice);

  D3D9CALL(mDevice->GetDeviceCaps(&mDeviceCaps));
  ImGui_ImplDX9_Init(mDevice.Get());
}

D3D9Renderer::~D3D9Renderer() {
  ImGui_ImplDX9_Shutdown();
}

void D3D9Renderer::before_reset() {
  ImGui_ImplDX9_InvalidateDeviceObjects();
}

void D3D9Renderer::after_reset() {
  ImGui_ImplDX9_CreateDeviceObjects();
}

/*
 * Stores the vertex data into a new static vertex buffer in the managed pool.
 */
auto D3D9Renderer::add_mesh(
  void* data, const i32 numVertices, const VertexLayout& layout
, const PrimitiveType primitiveType
) -> MeshHandle {
  BASALT_ASSERT(data);
  BASALT_ASSERT(numVertices > 0);
  BASALT_ASSERT(!layout.empty());

  const auto [meshHandle, mesh] = mMeshes.allocate();
  mesh.fvf = to_fvf(layout);
  BASALT_ASSERT_MSG(
    verify_fvf(mesh.fvf), "invalid fvf. Consult the log for details");

  mesh.vertexSize = ::D3DXGetFVFVertexSize(mesh.fvf);

  const auto bufferSize = mesh.vertexSize * numVertices;
  D3D9CALL(
    mDevice->CreateVertexBuffer(bufferSize, D3DUSAGE_WRITEONLY, mesh.fvf,
      D3DPOOL_MANAGED, mesh.vertexBuffer.GetAddressOf(), nullptr));

  // upload vertex data
  void* vertexBufferData = nullptr;
  if (SUCCEEDED(mesh.vertexBuffer->Lock(0u, 0u, &vertexBufferData, 0u))) {
    std::memcpy(vertexBufferData, data, bufferSize);
    D3D9CALL(mesh.vertexBuffer->Unlock());
  } else {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");
  }

  fill_primitive_info(mesh, primitiveType, numVertices);

  return meshHandle;
}

void D3D9Renderer::remove_mesh(const MeshHandle meshHandle) {
  auto& mesh = mMeshes.get(meshHandle);

  mesh.vertexBuffer.Reset();

  mMeshes.deallocate(meshHandle);
}

auto D3D9Renderer::add_texture(const std::string_view filePath) -> TextureHandle {
  const auto wideFilePath = win32::create_wide_from_utf8(filePath);

  IDirect3DTexture9* texture = nullptr;
  if (FAILED(::D3DXCreateTextureFromFileW(
    mDevice.Get(), wideFilePath.c_str(), &texture
  ))) {
    throw std::runtime_error("loading texture file failed");
  }

  const auto [texHandle, tex] = mTextures.allocate();
  tex = texture;

  return texHandle;
}

void D3D9Renderer::remove_texture(const TextureHandle textureHandle) {
  mTextures.get(textureHandle).Reset();
  mTextures.deallocate(textureHandle);
}

void D3D9Renderer::set_clear_color(const Color& color) {
  mClearColor = to_d3d_color(color);
}

// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void D3D9Renderer::render(const RenderCommandList& commandList) {
  const auto hr = mDevice->TestCooperativeLevel();
  if (hr == D3DERR_DEVICENOTRESET) {
    BASALT_LOG_INFO("resetting d3d9 device");
    D3D9CALL(mDevice->Reset(&mPresentParams));

    ImGui_ImplDX9_CreateDeviceObjects();
  } else if (hr != D3DERR_DEVICELOST) {
    BASALT_ASSERT(SUCCEEDED(hr));
  }

  D3D9CALL(
    mDevice->Clear(0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, mClearColor,
      1.0f, 0u));

  // TODO: should we make all rendering code dependent
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CALL(mDevice->BeginScene());

  auto transform {to_d3d_matrix(commandList.view())};
  D3D9CALL(mDevice->SetTransform(D3DTS_VIEW, &transform));

  transform = to_d3d_matrix(commandList.projection());

  BASALT_ASSERT_MSG(
    transform._34 >= 0, "(3,4) can't be negative in a projection matrix");

  D3D9CALL(mDevice->SetTransform(D3DTS_PROJECTION, &transform));

  const auto ambientLightColor = to_d3d_color(commandList.ambient_light());
  if (ambientLightColor) {
    D3D9CALL(mDevice->SetRenderState(D3DRS_AMBIENT, ambientLightColor));
  }

  const auto& directionalLights = commandList.directional_lights();
  BASALT_ASSERT_MSG(
    directionalLights.size() <= mDeviceCaps.MaxActiveLights
  , "the renderer doesn't support that many lights");

  DWORD lightIndex = 0u;
  for (const auto& light : directionalLights) {
    D3DLIGHT9 d3dLight {D3DLIGHT_DIRECTIONAL};
    d3dLight.Diffuse = to_d3d_color_value(light.diffuseColor);
    d3dLight.Ambient = to_d3d_color_value(light.ambientColor);
    d3dLight.Direction = to_d3d_vector(light.direction);

    D3D9CALL(mDevice->SetLight(lightIndex, &d3dLight));
    D3D9CALL(mDevice->LightEnable(lightIndex++, TRUE));
  }

  for (const auto& command : commandList.commands()) {
    render_command(command);
  }

  // render imgui
  ImGui::Render();
  auto* drawData = ImGui::GetDrawData();
  if (drawData) {
    ImGui_ImplDX9_RenderDrawData(drawData);
  }

  // disable used lights
  for (lightIndex = 0; lightIndex < directionalLights.size(); lightIndex++) {
    D3D9CALL(mDevice->LightEnable(lightIndex, FALSE));
  }

  if (ambientLightColor) {
    D3D9CALL(mDevice->SetRenderState(D3DRS_AMBIENT, 0u));
  }

  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));

  D3D9CALL(mDevice->EndScene());
}

void D3D9Renderer::new_gui_frame() {
  ImGui_ImplDX9_NewFrame();
}

void D3D9Renderer::render_command(const RenderCommand& command) {
  const bool disableLighting = command.mFlags & RenderFlagDisableLighting;

  // apply custom render flags
  if (command.mFlags) {
    if (disableLighting) {
      D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
    }
    if (command.mFlags & RenderFlagCullNone) {
      D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    }
  }

  const auto& mesh = mMeshes.get(command.mMesh);
  const bool noLightingAndTransform = mesh.fvf & D3DFVF_XYZRHW;
  if (!disableLighting && !noLightingAndTransform) {
    D3DMATERIAL9 material {};
    material.Diffuse = to_d3d_color_value(command.mDiffuseColor);
    material.Ambient = to_d3d_color_value(command.mAmbientColor);
    material.Emissive = to_d3d_color_value(command.mEmissiveColor);
    D3D9CALL(mDevice->SetMaterial(&material));
  }

  if (command.mTexture) {
    const auto& texture = mTextures.get(command.mTexture);
    D3D9CALL(mDevice->SetTexture(0, texture.Get()));
    D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
  }

  if (!noLightingAndTransform) {
    const auto transform {to_d3d_matrix(command.mWorld)};
    D3D9CALL(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &transform));
  }

  D3D9CALL(
    mDevice->SetStreamSource(0u, mesh.vertexBuffer.Get(), 0u, mesh.vertexSize));

  D3D9CALL(mDevice->SetFVF(mesh.fvf));
  D3D9CALL(mDevice->DrawPrimitive(mesh.primType, 0u, mesh.primCount));

  if (command.mTexture) {
    D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
    D3D9CALL(mDevice->SetTexture(0, nullptr));
  }

  // revert custom render flags
  if (command.mFlags) {
    if (disableLighting) {
      D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
    }
    if (command.mFlags & RenderFlagCullNone) {
      D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
    }
  }
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

      case VertexElement::ColorDiffuse1U32:
        fvf |= D3DFVF_DIFFUSE;
        break;

      case VertexElement::ColorSpecular1U32:
        fvf |= D3DFVF_SPECULAR;
        break;

      case VertexElement::TextureCoords2F32:
        fvf |= D3DFVF_TEX1;
        break;
    }
  }

  return fvf;
}

auto verify_fvf(const DWORD fvf) -> bool {
  if (fvf & D3DFVF_XYZRHW && (fvf & D3DFVF_XYZ || fvf & D3DFVF_NORMAL)) {
    BASALT_LOG_ERROR("can't use transformed positions with untransformed"
      "positions or normals");
    return false;
  }

  return true;
}

void fill_primitive_info(
  D3D9Mesh& mesh, const PrimitiveType primitiveType, const i32 numVtx
) {
  switch (primitiveType) {
  case PrimitiveType::PointList:
    mesh.primType = D3DPT_POINTLIST;
    mesh.primCount = numVtx;
    break;

  case PrimitiveType::LineList:
    mesh.primType = D3DPT_LINELIST;
    BASALT_ASSERT_MSG(
      numVtx % 2 == 0, "Wrong amount of vertices for PrimitiveType::LINE_LIST");
    mesh.primCount = numVtx / 2;
    break;

  case PrimitiveType::LineStrip:
    mesh.primType = D3DPT_LINESTRIP;
    mesh.primCount = numVtx - 1;
    break;

  case PrimitiveType::TriangleList:
    mesh.primType = D3DPT_TRIANGLELIST;
    BASALT_ASSERT_MSG(
      numVtx % 3 == 0
    , "Wrong amount of vertices for PrimitiveType::TRIANGLE_LIST");
    mesh.primCount = numVtx / 3;
    break;

  case PrimitiveType::TriangleStrip:
    mesh.primType = D3DPT_TRIANGLESTRIP;
    mesh.primCount = numVtx - 2;
    break;

  case PrimitiveType::TriangleFan:
    mesh.primType = D3DPT_TRIANGLEFAN;
    mesh.primCount = numVtx - 2;
    break;
  }
}

} // namespace

} // namespace basalt::gfx::backend
