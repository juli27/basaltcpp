#include "D3D9Renderer.h"

#include "D3D9Util.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Color.h"
#include "runtime/shared/Log.h"

#include "runtime/shared/win32/Win32SharedUtil.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <stdexcept>
#include <string>

#include <cstring> // for memcpy

namespace basalt::gfx::backend {

using platform::WindowResizedEvent;

namespace {

constexpr std::string_view RENDERER_NAME = "Direct3D 9 fixed function";

auto vertex_layout_to_fvf(const VertexLayout& layout) -> DWORD {
  DWORD fvf = 0u;

  for (const auto& element : layout.get_elements()) {
    switch (element.mUsage) {
      case VertexElementUsage::Position:
        if (element.mType != VertexElementType::F32_3) {
          throw std::runtime_error("vertex layout not supported");
        }
        if (fvf & D3DFVF_XYZRHW) {
          throw std::runtime_error(
            "can't use transformed positions with untransformed"
            "positions or normals"
          );
        }
        fvf |= D3DFVF_XYZ;
        break;

      case VertexElementUsage::Normal:
        if (element.mType != VertexElementType::F32_3) {
          throw std::runtime_error("vertex layout not supported");
        }
        if (fvf & D3DFVF_XYZRHW) {
          throw std::runtime_error(
            "can't use transformed positions with untransformed"
            "positions or normals"
          );
        }
        fvf |= D3DFVF_NORMAL;
        break;

      case VertexElementUsage::TextureCoords:
        if (element.mType != VertexElementType::F32_2) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_TEX1;
        break;

      case VertexElementUsage::ColorDiffuse:
        if (element.mType != VertexElementType::U32_1) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_DIFFUSE;
        break;


      case VertexElementUsage::ColorSpecular:
        if (element.mType != VertexElementType::U32_1) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_SPECULAR;
        break;

      case VertexElementUsage::PositionTransformed:
        if (element.mType != VertexElementType::F32_4) {
          throw std::runtime_error("vertex layout not supported");
        }
        if (fvf & D3DFVF_XYZ || fvf & D3DFVF_NORMAL) {
          throw std::runtime_error(
            "can't use transformed positions with untransformed"
            "positions or normals"
          );
        }
        fvf |= D3DFVF_XYZRHW;
        break;

      default:
        throw std::runtime_error("vertex layout not supported");
    }
  }

  return fvf;
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
      BASALT_ASSERT(
        numVtx % 2 == 0,
        "Wrong amount of vertices for PrimitiveType::LINE_LIST"
      );
      mesh.primCount = numVtx / 2;
      break;

    case PrimitiveType::LineStrip:
      mesh.primType = D3DPT_LINESTRIP;
      mesh.primCount = numVtx - 1;
      break;

    case PrimitiveType::TriangleList:
      mesh.primType = D3DPT_TRIANGLELIST;
      BASALT_ASSERT(
        numVtx % 3 == 0,
        "Wrong amount of vertices for PrimitiveType::TRIANGLE_LIST"
      );
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

    default:
      throw std::runtime_error("primitive type not supported");
  }
}

void fill_color(D3DCOLORVALUE& d3dColor, const Color color) {
  d3dColor.r = color.red() / 255.0f;
  d3dColor.g = color.green() / 255.0f;
  d3dColor.b = color.blue() / 255.0f;
  d3dColor.a = color.alpha() / 255.0f;
}

} // namespace

D3D9Renderer::D3D9Renderer(IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& pp)
: mDevice(device)
, mPresentParams(pp) {
  BASALT_ASSERT(device, "device is null");
  mDevice->AddRef();
  D3D9CALL(mDevice->GetDeviceCaps(&mDeviceCaps));

  ImGui_ImplDX9_Init(mDevice);
}

D3D9Renderer::~D3D9Renderer() {
  ImGui_ImplDX9_Shutdown();

  mTextures.for_each([](D3D9Texture& texture){
    texture.texture->Release();
  });
  mMeshes.for_each([](D3D9Mesh& mesh){
    mesh.vertexBuffer->Release();
  });

  mDevice->Release();
}

void D3D9Renderer::on_window_resize(const WindowResizedEvent& event) {
  ImGui_ImplDX9_InvalidateDeviceObjects();
  mPresentParams.BackBufferWidth = event.mNewSize.width();
  mPresentParams.BackBufferHeight = event.mNewSize.height();
  mDevice->Reset(&mPresentParams);
  ImGui_ImplDX9_CreateDeviceObjects();

  BASALT_LOG_TRACE("resized d3d9 back buffer");
}

/*
 * Stores the vertex data into a new static vertex buffer in the managed pool.
 */
auto D3D9Renderer::add_mesh(
  void* data, const i32 numVertices, const VertexLayout& layout,
  const PrimitiveType primitiveType
) -> MeshHandle {
  BASALT_ASSERT_ARG_NOT_NULL(data);
  BASALT_ASSERT(numVertices > 0, "numVertices must be > 0");
  BASALT_ASSERT(!layout.get_elements().empty(), "must specify a vertex layout");

  const auto fvf = vertex_layout_to_fvf(layout);
  const auto vertexSize = D3DXGetFVFVertexSize(fvf);
  const auto bufferSize = vertexSize * numVertices;

  IDirect3DVertexBuffer9* vertexBuffer = nullptr;
  D3D9CALL(mDevice->CreateVertexBuffer(
    bufferSize, D3DUSAGE_WRITEONLY, fvf, D3DPOOL_MANAGED, &vertexBuffer,
    nullptr
  ));

  // upload vertex data
  void* vertexBufferData = nullptr;
  if (SUCCEEDED(vertexBuffer->Lock(0u, 0u, &vertexBufferData, 0u))) {
    std::memcpy(vertexBufferData, data, bufferSize);
    D3D9CALL(vertexBuffer->Unlock());
  } else {
    BASALT_LOG_ERROR("Failed to lock vertex buffer");
  }

  const auto [meshHandle, mesh] = mMeshes.allocate();
  mesh.vertexBuffer = vertexBuffer;
  mesh.fvf = fvf;
  mesh.vertexSize = vertexSize;
  fill_primitive_info(mesh, primitiveType, numVertices);

  return meshHandle;
}

void D3D9Renderer::remove_mesh(const MeshHandle meshHandle) {
  auto& mesh = mMeshes.get(meshHandle);

  mesh.vertexBuffer->Release();
  mesh.vertexBuffer = nullptr;

  mMeshes.deallocate(meshHandle);
}

auto D3D9Renderer::add_texture(const std::string_view filePath) -> TextureHandle {
  const auto wideFilePath = create_wide_from_utf8(filePath);

  IDirect3DTexture9* texture = nullptr;
  if (FAILED(::D3DXCreateTextureFromFileW(
    mDevice, wideFilePath.c_str(), &texture
  ))) {
    throw std::runtime_error("loading texture file failed");
  }

  const auto [texHandle, tex] = mTextures.allocate();
  tex.texture = texture;

  return texHandle;
}

void D3D9Renderer::remove_texture(const TextureHandle textureHandle) {
  auto& texture = mTextures.get(textureHandle);

  texture.texture->Release();
  texture.texture = nullptr;

  mTextures.deallocate(textureHandle);
}

void D3D9Renderer::submit(const RenderCommand& command) {
  mCommandBuffer.add_command(command);
}

void D3D9Renderer::set_view_proj(
  const math::Mat4f32& view, const math::Mat4f32& projection
) {
  BASALT_ASSERT(projection.m34 >= 0,
    "m34 can't be negative in a projection matrix");

  mCommandBuffer.set_view(view);
  mCommandBuffer.set_projection(projection);
}

void D3D9Renderer::set_lights(const LightSetup& lights) {
  const auto maxLights = mDeviceCaps.MaxActiveLights;

  const auto& directionalLights = lights.get_directional_lights();
  if (directionalLights.size() > maxLights) {
    throw std::runtime_error("the renderer doesn't support that many lights");
  }

  DWORD lightIndex = 0u;
  for (const auto& light : directionalLights) {
    D3DLIGHT9 d3dlight{};
    d3dlight.Type = D3DLIGHT_DIRECTIONAL;
    d3dlight.Direction = *reinterpret_cast<const D3DVECTOR*>(&light.mDirection);
    fill_color(d3dlight.Diffuse, light.mDiffuseColor);
    fill_color(d3dlight.Ambient, light.mAmbientColor);

    D3D9CALL(mDevice->SetLight(lightIndex, &d3dlight));
    D3D9CALL(mDevice->LightEnable(lightIndex, TRUE));
    lightIndex++;
  }

  // disable not used lights
  for (; lightIndex < maxLights; lightIndex++) {
    D3D9CALL(mDevice->LightEnable(lightIndex, FALSE));
  }

  D3D9CALL(mDevice->SetRenderState(
    D3DRS_AMBIENT, lights.get_global_ambient_color().to_argb()
  ));
}

void D3D9Renderer::set_clear_color(const Color color) {
  mClearColor = color.to_argb();
}

// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void D3D9Renderer::render() {
  const auto hr = mDevice->TestCooperativeLevel();
  if (hr == D3DERR_DEVICENOTRESET) {
    BASALT_LOG_INFO("resetting d3d9 device");
    D3D9CALL(mDevice->Reset(&mPresentParams));
    ImGui_ImplDX9_CreateDeviceObjects();
  } else if (hr != D3DERR_DEVICELOST) {
    BASALT_ASSERT(SUCCEEDED(hr), "");
  }

  D3D9CALL(mDevice->Clear(
    0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, mClearColor, 1.0f, 0u
  ));

  // TODO: should we make all rendering code dependant
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CALL(mDevice->BeginScene());

  D3D9CALL(mDevice->SetTransform(
    D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.view())
  ));
  D3D9CALL(mDevice->SetTransform(
    D3DTS_PROJECTION,
    reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.projection())
  ));

  render_commands(mCommandBuffer);

  // render imgui
  ImGui::Render();
  auto* drawData = ImGui::GetDrawData();
  if (drawData) {
    ImGui_ImplDX9_RenderDrawData(drawData);
  }

  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));

  D3D9CALL(mDevice->EndScene());

  mCommandBuffer.clear();
}

auto D3D9Renderer::name() -> std::string_view {
  return RENDERER_NAME;
}


void D3D9Renderer::new_gui_frame() {
  ImGui_ImplDX9_NewFrame();
}

void D3D9Renderer::render_commands(const RenderCommandBuffer& commands) {
  for (const auto& command : commands.get_commands()) {
    // apply custom render flags
    if (command.mFlags) {
      if (command.mFlags & RenderFlagDisableLighting) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
      }
      if (command.mFlags & RenderFlagCullNone) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
      }
    }

    D3DMATERIAL9 material{};
    fill_color(material.Diffuse, command.mDiffuseColor);
    fill_color(material.Ambient, command.mAmbientColor);
    fill_color(material.Emissive, command.mEmissiveColor);
    D3D9CALL(mDevice->SetMaterial(&material));

    if (command.mTexture) {
      const auto& texture = mTextures.get(command.mTexture);
      D3D9CALL(mDevice->SetTexture(0, texture.texture));
    } else {
      D3D9CALL(mDevice->SetTexture(0, nullptr));
    }

    const auto& mesh = mMeshes.get(command.mMesh);
    D3D9CALL(mDevice->SetStreamSource(
      0u, mesh.vertexBuffer, 0u, mesh.vertexSize
    ));
    D3D9CALL(mDevice->SetFVF(mesh.fvf));

    D3D9CALL(mDevice->SetTransform(
      D3DTS_WORLDMATRIX(0), reinterpret_cast<const D3DMATRIX*>(&command.mWorld)
    ));

    D3D9CALL(mDevice->DrawPrimitive(mesh.primType, 0u, mesh.primCount));

    // revert custom render flags
    if (command.mFlags) {
      if (command.mFlags & RenderFlagDisableLighting) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
      }
      if (command.mFlags & RenderFlagCullNone) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
      }
    }
  }
}

} // namespace basalt::gfx::backend
