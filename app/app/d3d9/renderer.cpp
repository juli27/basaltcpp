#include "renderer.h"

#include "util.h"
#include "app/shared/util.h"

#include "runtime/math/Mat4.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Color.h"
#include "runtime/shared/Log.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <array>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <utility>

using std::array;

using Microsoft::WRL::ComPtr;

namespace basalt::gfx::backend {

using math::Mat4f32;

namespace {

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

constexpr auto to_d3d_color_value(
  const Color& color) noexcept -> D3DCOLORVALUE {
  return {color.red(), color.green(), color.blue(), color.alpha()};
}

constexpr auto to_d3d_matrix(const Mat4f32& mat) noexcept -> D3DMATRIX {
  return {
    mat.m11, mat.m12, mat.m13, mat.m14
  , mat.m21, mat.m22, mat.m23, mat.m24
  , mat.m31, mat.m32, mat.m33, mat.m34
  , mat.m41, mat.m42, mat.m43, mat.m44
  };
}

constexpr auto to_d3d_vector(const math::Vec3f32& vec) noexcept -> D3DVECTOR {
  return {vec.x, vec.y, vec.z};
}

auto to_fvf(const VertexLayout& layout) -> DWORD;
auto verify_fvf(DWORD fvf) -> bool;

void fill_primitive_info(
  D3D9Mesh& mesh, PrimitiveType primitiveType, i32 numVtx);

} // namespace

D3D9Renderer::D3D9Renderer(ComPtr<IDirect3DDevice9> device)
  : mDevice {std::move(device)} {
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
  const auto [handle, texture] = mTextures.allocate();

  const auto wideFilePath = win32::create_wide_from_utf8(filePath);
  if (FAILED(::D3DXCreateTextureFromFileW(
    mDevice.Get(), wideFilePath.c_str(), texture.GetAddressOf()
  ))) {
    throw std::runtime_error("loading texture file failed");
  }

  return handle;
}

void D3D9Renderer::remove_texture(const TextureHandle textureHandle) {
  mTextures.get(textureHandle).Reset();
  mTextures.deallocate(textureHandle);
}

auto D3D9Renderer::load_model(const std::string_view filePath) -> ModelHandle {
  const auto [handle, model] = mModels.allocate();

  const auto wideFilePath {win32::create_wide_from_utf8(filePath)};

  ComPtr<ID3DXBuffer> materialBuffer {};
  DWORD numMaterials {};
  auto hr = ::D3DXLoadMeshFromXW(
    wideFilePath.c_str(), D3DXMESH_MANAGED, mDevice.Get(), nullptr
  , materialBuffer.GetAddressOf(), nullptr, &numMaterials
  , model.mesh.GetAddressOf());
  BASALT_ASSERT(SUCCEEDED(hr));

  auto const* materials = static_cast<D3DXMATERIAL*>(materialBuffer->GetBufferPointer());
  model.materials.reserve(numMaterials);
  model.textures.resize(numMaterials);

  for (DWORD i = 0; i < numMaterials; i++) {
    model.materials.push_back(materials->MatD3D);

    // d3dx doesn't set the ambient color
    model.materials[i].Ambient = model.materials[i].Diffuse;

    if (materials[i].pTextureFilename != nullptr && lstrlenA(
      materials[i].pTextureFilename) > 0) {
      array<char, MAX_PATH> texPath {};
      strcpy_s(texPath.data(), texPath.size(), "data/");
      strcat_s(texPath.data(), texPath.size(), materials[i].pTextureFilename);

      hr = ::D3DXCreateTextureFromFileA(
        mDevice.Get(), texPath.data(), model.textures[i].GetAddressOf());
      BASALT_ASSERT(SUCCEEDED(hr));
    }
  }

  return handle;
}

void D3D9Renderer::remove_model(const ModelHandle handle) {
  auto& model = mModels.get(handle);
  model.mesh.Reset();
  model.textures.clear();
  model.materials.clear();

  mModels.deallocate(handle);
}

void D3D9Renderer::set_clear_color(const Color& color) {
  mClearColor = to_d3d_color(color);
}

// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void D3D9Renderer::render(const RenderCommandList& commandList) {
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

  if (command.model) {
    const auto transform {to_d3d_matrix(command.mWorld)};
      D3D9CALL(mDevice->SetTransform(D3DTS_WORLDMATRIX(0), &transform));

    const auto& model = mModels.get(command.model);
    for (DWORD i = 0; i < model.materials.size(); i++) {
      D3D9CALL(mDevice->SetMaterial(&model.materials[i]));
      D3D9CALL(mDevice->SetTexture(0, model.textures[i].Get()));

      model.mesh->DrawSubset(i);
    }

    D3D9CALL(mDevice->SetTexture(0, nullptr));

  } else {
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

      // transform tex coords
      if (command.texTransform != Mat4f32::identity()) {
        const D3DMATRIX texTransform {to_d3d_matrix(command.texTransform)};
        D3D9CALL(mDevice->SetTransform(D3DTS_TEXTURE0, &texTransform));

        D3D9CALL(
          mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
            D3DTTFF_COUNT4 | D3DTTFF_PROJECTED));
      }

      // set texture coordinate index
      DWORD tci {0};
      switch (command.texCoordinateSrc) {
      case TexCoordinateSrc::PositionCameraSpace:
        tci = D3DTSS_TCI_CAMERASPACEPOSITION;
        break;

      case TexCoordinateSrc::Vertex:
        break;
      }

      if (!(mesh.fvf & D3DFVF_TEX1)) {
        D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, tci));
      }
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
      // revert TCI usage
      if (command.texCoordinateSrc != TexCoordinateSrc::Vertex) {
        D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
      }

      // revert tex coords transform
      if (command.texTransform != Mat4f32::identity()) {
        D3D9CALL(
          mDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
            D3DTTFF_DISABLE));

        const D3DMATRIX identity {to_d3d_matrix(Mat4f32::identity())};
        D3D9CALL(mDevice->SetTransform(D3DTS_TEXTURE0, &identity));
      }

      D3D9CALL(mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
      D3D9CALL(mDevice->SetTexture(0, nullptr));
    }
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