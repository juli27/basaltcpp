#include <basalt/gfx/backend/d3d9/Renderer.h>

#include <cstring> // for memcpy

#include <limits>
#include <stdexcept>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <basalt/common/Asserts.h>
#include <basalt/common/Color.h>
#include <basalt/gfx/backend/d3d9/Util.h>
#include <basalt/platform/Platform.h>
#include <basalt/platform/events/WindowEvents.h>

namespace basalt::gfx::backend::d3d9 {
namespace {

constexpr std::string_view RENDERER_NAME = "Direct3D 9 fixed function";

auto TranslateVertexLayoutToFVF(const VertexLayout& layout) -> DWORD {
  DWORD fvf = 0u;

  for (const auto& element : layout.GetElements()) {
    switch (element.usage) {
      case VertexElementUsage::POSITION:
        if (element.type != VertexElementType::F32_3) {
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

      case VertexElementUsage::NORMAL:
        if (element.type != VertexElementType::F32_3) {
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

      case VertexElementUsage::TEXTURE_COORDS:
        if (element.type != VertexElementType::F32_2) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_TEX1;
        break;

      case VertexElementUsage::COLOR_DIFFUSE:
        if (element.type != VertexElementType::U32_1) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_DIFFUSE;
        break;


      case VertexElementUsage::COLOR_SPECULAR:
        if (element.type != VertexElementType::U32_1) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_SPECULAR;
        break;

      case VertexElementUsage::POSITION_TRANSFORMED:
        if (element.type != VertexElementType::F32_4) {
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

void FillPrimitiveInfo(
  Mesh& mesh, const PrimitiveType primitiveType, const i32 numVtx
) {
  switch (primitiveType) {
    case PrimitiveType::POINT_LIST:
      mesh.primType = D3DPT_POINTLIST;
      mesh.primCount = numVtx;
      break;

    case PrimitiveType::LINE_LIST:
      mesh.primType = D3DPT_LINELIST;
      BS_ASSERT(
        numVtx % 2 == 0,
        "Wrong amount of vertices for PrimitiveType::LINE_LIST"
      );
      mesh.primCount = numVtx / 2;
      break;

    case PrimitiveType::LINE_STRIP:
      mesh.primType = D3DPT_LINESTRIP;
      mesh.primCount = numVtx - 1;
      break;

    case PrimitiveType::TRIANGLE_LIST:
      mesh.primType = D3DPT_TRIANGLELIST;
      BS_ASSERT(
        numVtx % 3 == 0,
        "Wrong amount of vertices for PrimitiveType::TRIANGLE_LIST"
      );
      mesh.primCount = numVtx / 3;
      break;

    case PrimitiveType::TRIANGLE_STRIP:
      mesh.primType = D3DPT_TRIANGLESTRIP;
      mesh.primCount = numVtx - 2;
      break;

    case PrimitiveType::TRIANGLE_FAN:
      mesh.primType = D3DPT_TRIANGLEFAN;
      mesh.primCount = numVtx - 2;
      break;

    default:
      throw std::runtime_error("primitive type not supported");
  }
}

void FillD3DColor(D3DCOLORVALUE& d3dColor, const Color color) {
  d3dColor.r = color.GetRed() / 255.0f;
  d3dColor.g = color.GetGreen() / 255.0f;
  d3dColor.b = color.GetBlue() / 255.0f;
  d3dColor.a = color.GetAlpha() / 255.0f;
}

auto CreateWideFromUTF8(const std::string_view source) -> std::wstring {
  if (source.empty()) {
    return std::wstring();
  }

  BS_ASSERT(
    source.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()),
    "string too large"
  );
  const int sourceSize = static_cast<int>(source.size());
  const int size = ::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), sourceSize, nullptr, 0
  );

  if (size == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  std::wstring dest(size, '\0');
  if (::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), sourceSize, dest.data(),
    static_cast<int>(dest.size())
  ) == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  return dest;
}

} // namespace

Renderer::Renderer(IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& pp)
: mDevice(device)
, mPresentParams(pp) {
  BS_ASSERT(device, "device is null");
  mDevice->AddRef();
  D3D9CALL(mDevice->GetDeviceCaps(&mDeviceCaps));

  ImGui_ImplDX9_Init(mDevice);

  platform::add_event_listener([this](const platform::Event& e) {
    const platform::EventDispatcher dispatcher(e);
    dispatcher.dispatch<platform::WindowResizedEvent>(
      [this](const platform::WindowResizedEvent& event) {
      ImGui_ImplDX9_InvalidateDeviceObjects();
      mPresentParams.BackBufferWidth = event.mNewSize.get_x();
      mPresentParams.BackBufferHeight = event.mNewSize.get_y();
      mDevice->Reset(&mPresentParams);
      ImGui_ImplDX9_CreateDeviceObjects();

      BS_INFO("resizing d3d9 back buffer");
    });
  });
}

Renderer::~Renderer() {
  ImGui_ImplDX9_Shutdown();

  mTextures.ForEach([](Texture& texture){
    texture.texture->Release();
  });
  mMeshes.ForEach([](Mesh& mesh){
    mesh.vertexBuffer->Release();
  });

  mDevice->Release();
}

/*
 * Stores the vertex data into a new static vertex buffer in the managed pool.
 */
auto Renderer::AddMesh(
  void* data, i32 numVertices, const VertexLayout& layout,
  PrimitiveType primitiveType
) -> MeshHandle {
  BS_ASSERT_ARG_NOT_NULL(data);
  BS_ASSERT(numVertices > 0, "numVertices must be > 0");
  BS_ASSERT(!layout.GetElements().empty(), "must specify a vertex layout");


  const DWORD fvf = TranslateVertexLayoutToFVF(layout);
  const UINT vertexSize = D3DXGetFVFVertexSize(fvf);
  const UINT bufferSize = vertexSize * numVertices;

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
    BS_ERROR("Failed to lock vertex buffer");
  }

  const MeshHandle meshHandle = mMeshes.Allocate();
  Mesh& mesh = mMeshes.Get(meshHandle);
  mesh.vertexBuffer = vertexBuffer;
  mesh.fvf = fvf;
  mesh.vertexSize = vertexSize;
  FillPrimitiveInfo(mesh, primitiveType, numVertices);

  return meshHandle;
}


void Renderer::RemoveMesh(MeshHandle meshHandle) {
  Mesh& mesh = mMeshes.Get(meshHandle);

  mesh.vertexBuffer->Release();
  mesh.vertexBuffer = nullptr;

  mMeshes.Deallocate(meshHandle);
}


auto Renderer::AddTexture(const std::string_view filePath) -> TextureHandle {
  const std::wstring wideFilePath = CreateWideFromUTF8(filePath);

  IDirect3DTexture9* texture = nullptr;
  if (FAILED(::D3DXCreateTextureFromFileW(
    mDevice, wideFilePath.c_str(), &texture
  ))) {
    throw std::runtime_error("loading texture file failed");
  }

  const TextureHandle texHandle = mTextures.Allocate();
  Texture& tex = mTextures.Get(texHandle);
  tex.texture = texture;

  return texHandle;
}


void Renderer::RemoveTexture(TextureHandle textureHandle) {
  Texture& texture = mTextures.Get(textureHandle);

  texture.texture->Release();
  texture.texture = nullptr;

  mTextures.Deallocate(textureHandle);
}


void Renderer::Submit(const RenderCommand& command) {
  mCommandBuffer.AddCommand(command);
}


void Renderer::SetViewProj(
  const math::Mat4f32& view, const math::Mat4f32& projection
) {
  if (projection.m34 < 0) {
    throw std::runtime_error("m34 can't be negative in a projection matrix");
  }

  mCommandBuffer.SetView(view);
  mCommandBuffer.SetProjection(projection);
}

void Renderer::SetLights(const LightSetup& lights) {
  const DWORD MAX_LIGHTS = mDeviceCaps.MaxActiveLights;

  const auto& directionalLights = lights.GetDirectionalLights();
  if (directionalLights.size() > MAX_LIGHTS) {
    throw std::runtime_error("the renderer doesn't support that many lights");
  }

  DWORD lightIndex = 0u;
  for (const DirectionalLight& light : directionalLights) {
    D3DLIGHT9 d3dlight{};
    d3dlight.Type = D3DLIGHT_DIRECTIONAL;
    d3dlight.Direction = *reinterpret_cast<const D3DVECTOR*>(&light.direction);
    FillD3DColor(d3dlight.Diffuse, light.diffuseColor);
    FillD3DColor(d3dlight.Ambient, light.ambientColor);

    D3D9CALL(mDevice->SetLight(lightIndex, &d3dlight));
    D3D9CALL(mDevice->LightEnable(lightIndex, TRUE));
    lightIndex++;
  }

  // disable not used lights
  for (; lightIndex < MAX_LIGHTS; lightIndex++) {
    D3D9CALL(mDevice->LightEnable(lightIndex, FALSE));
  }

  D3D9CALL(mDevice->SetRenderState(
    D3DRS_AMBIENT, lights.GetGlobalAmbientColor().ToARGB()
  ));
}


void Renderer::SetClearColor(Color color) {
  mClearColor = color.ToARGB();
}


// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void Renderer::Render() {
  const auto hr = mDevice->TestCooperativeLevel();
  if (hr == D3DERR_DEVICENOTRESET) {
    BS_INFO("resetting d3d9 device");
    D3D9CALL(mDevice->Reset(&mPresentParams));
    ImGui_ImplDX9_CreateDeviceObjects();
  } else if (hr != D3DERR_DEVICELOST) {
    BS_ASSERT(SUCCEEDED(hr), "");
  }

  D3D9CALL(mDevice->Clear(
    0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, mClearColor, 1.0f, 0u
  ));

  // TODO: should we make all rendering code dependant
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CALL(mDevice->BeginScene());

  D3D9CALL(mDevice->SetTransform(
    D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.GetView())
  ));
  D3D9CALL(mDevice->SetTransform(
    D3DTS_PROJECTION,
    reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.GetProjection())
  ));

  RenderCommands(mCommandBuffer);

  // render imgui
  ImGui::Render();
  auto* drawData = ImGui::GetDrawData();
  if (drawData) {
    ImGui_ImplDX9_RenderDrawData(drawData);
  }

  D3D9CALL(mDevice->SetStreamSource(0u, nullptr, 0u, 0u));

  D3D9CALL(mDevice->EndScene());

  mCommandBuffer.Clear();
}


void Renderer::Present() {
  const auto hr = mDevice->Present(nullptr, nullptr, nullptr, nullptr);
  if (hr == D3DERR_DEVICELOST) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
  } else {
    BS_ASSERT(SUCCEEDED(hr), "");
  }
}


auto Renderer::GetName() -> std::string_view {
  return RENDERER_NAME;
}


void Renderer::NewGuiFrame() {
  ImGui_ImplDX9_NewFrame();
}


void Renderer::RenderCommands(const RenderCommandBuffer& commands) {
  for (const RenderCommand& command : commands.GetCommands()) {
    // apply custom render flags
    if (command.flags) {
      if (command.flags & RF_DISABLE_LIGHTING) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
      }
      if (command.flags & RF_CULL_NONE) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
      }
    }

    D3DMATERIAL9 material{};
    FillD3DColor(material.Diffuse, command.diffuseColor);
    FillD3DColor(material.Ambient, command.ambientColor);
    FillD3DColor(material.Emissive, command.emissiveColor);
    D3D9CALL(mDevice->SetMaterial(&material));

    if (command.texture) {
      const Texture& texture = mTextures.Get(command.texture);
      D3D9CALL(mDevice->SetTexture(0, texture.texture));
    } else {
      D3D9CALL(mDevice->SetTexture(0, nullptr));
    }

    const Mesh& mesh = mMeshes.Get(command.mesh);
    D3D9CALL(mDevice->SetStreamSource(
      0u, mesh.vertexBuffer, 0u, mesh.vertexSize
    ));
    D3D9CALL(mDevice->SetFVF(mesh.fvf));

    D3D9CALL(mDevice->SetTransform(
      D3DTS_WORLDMATRIX(0), reinterpret_cast<const D3DMATRIX*>(&command.world)
    ));

    D3D9CALL(mDevice->DrawPrimitive(mesh.primType, 0u, mesh.primCount));

    // revert custom render flags
    if (command.flags) {
      if (command.flags & RF_DISABLE_LIGHTING) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
      }
      if (command.flags & RF_CULL_NONE) {
        D3D9CALL(mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
      }
    }
  }
}

} // namespace basalt::gfx::backend::d3d9
