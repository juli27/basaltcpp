#include <basalt/gfx/backend/d3d9/Renderer.h>

#include <cstring> // for memcpy

#include <limits>
#include <stdexcept>

#include <basalt/common/Asserts.h>
#include <basalt/common/Color.h>
#include <basalt/gfx/backend/d3d9/Util.h>

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {
namespace {

constexpr std::string_view s_rendererName = "Direct3D 9 fixed function";


DWORD GetFVF(const VertexLayout& layout) {
  BS_ASSERT(layout.GetElements().size() != 0, "must specify a vertex layout");

  DWORD fvf = 0u;

  for (const VertexElement& element : layout.GetElements()) {
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
        break;
    }
  }

  return fvf;
}


void SetPrimitiveInfo(
  Mesh& mesh, PrimitiveType primitiveType, i32 numVertices
) {
  switch (primitiveType) {
    case PrimitiveType::POINT_LIST:
      mesh.primType = D3DPT_POINTLIST;
      mesh.primCount = numVertices;
      break;

    case PrimitiveType::LINE_LIST:
      mesh.primType = D3DPT_LINELIST;
      BS_ASSERT(
        numVertices % 2 == 0,
        "Wrong amount of vertices for PrimitiveType::LINE_LIST"
      );
      mesh.primCount = numVertices / 2;
      break;

    case PrimitiveType::LINE_STRIP:
      mesh.primType = D3DPT_LINESTRIP;
      mesh.primCount = numVertices - 1;
      break;

    case PrimitiveType::TRIANGLE_LIST:
      mesh.primType = D3DPT_TRIANGLELIST;
      BS_ASSERT(
        numVertices % 3 == 0,
        "Wrong amount of vertices for PrimitiveType::TRIANGLE_LIST"
      );
      mesh.primCount = numVertices / 3;
      break;

    case PrimitiveType::TRIANGLE_STRIP:
      mesh.primType = D3DPT_TRIANGLESTRIP;
      mesh.primCount = numVertices - 2;
      break;

    case PrimitiveType::TRIANGLE_FAN:
      mesh.primType = D3DPT_TRIANGLEFAN;
      mesh.primCount = numVertices - 2;
      break;

    default:
      throw std::runtime_error("primitive type not supported");
      break;
  }
}


void SetD3DColor(D3DCOLORVALUE& d3dColor, Color color) {
  d3dColor.r = color.GetRed() / 255.0f;
  d3dColor.g = color.GetGreen() / 255.0f;
  d3dColor.b = color.GetBlue() / 255.0f;
  d3dColor.a = color.GetAlpha() / 255.0f;
}


std::wstring CreateWideFromUTF8(const std::string_view source) {
  if (source.empty()) {
    return std::wstring();
  }

  const int size = ::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), source.size(), nullptr, 0
  );

  if (size == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  std::wstring dest(size, '\0');
  if (::MultiByteToWideChar(
    CP_UTF8, 0, source.data(), source.size(), dest.data(), dest.size()
  ) == 0) {
    throw std::runtime_error("MultiByteToWideChar failed");
  }

  return dest;
}

} // namespace


Renderer::Renderer(IDirect3DDevice9* device) : m_device(device) {
  BS_ASSERT_ARG_NOT_NULL(device);
  m_device->AddRef();

  // create default command buffer
  m_commandBuffers.emplace_back();
}


Renderer::~Renderer() {
  for (Texture& texture : m_textures) {
    texture.texture->Release();
  }

  for (Mesh& mesh : m_meshes) {
    mesh.vertexBuffer->Release();
  }

  m_device->Release();
}

/*
 * Stores the vertex data into a new static vertex buffer in the managed pool.
 */
MeshHandle Renderer::AddMesh(
  void* data, i32 numVertices, const VertexLayout& layout,
  PrimitiveType primitiveType
) {
  BS_ASSERT_ARG_NOT_NULL(data);
  BS_ASSERT(numVertices > 0, "numVertices must be > 0");

  const DWORD fvf = GetFVF(layout);
  const UINT vertexSize = D3DXGetFVFVertexSize(fvf);
  const UINT bufferSize = vertexSize * numVertices;

  IDirect3DVertexBuffer9* vertexBuffer = nullptr;
  D3D9CALL(m_device->CreateVertexBuffer(
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

  Mesh& mesh = GetFreeMeshSlot();
  mesh.vertexBuffer = vertexBuffer;
  mesh.fvf = fvf;
  mesh.vertexSize = vertexSize;
  SetPrimitiveInfo(mesh, primitiveType, numVertices);

  return mesh.handle;
}


void Renderer::RemoveMesh(MeshHandle meshHandle) {
  Mesh& mesh = GetMesh(meshHandle);

  mesh.vertexBuffer->Release();
  mesh.vertexBuffer = nullptr;

  const MeshHandle::IndexT index = mesh.handle.GetIndex();
  const MeshHandle::GenT gen = mesh.handle.GetGen() + 1;
  mesh.handle = MeshHandle(index, gen);

  // check if the slot reached "end of life"
  if (gen < std::numeric_limits<MeshHandle::GenT>::max()) {
    m_freeMeshSlots.push_back(index);
  }

  // reached end of life. don't add it to the free slots.
}


TextureHandle Renderer::AddTexture(std::string_view filePath) {
  const std::wstring wideFilePath = CreateWideFromUTF8(filePath);

  IDirect3DTexture9* texture = nullptr;
  if (FAILED(
    ::D3DXCreateTextureFromFileW(m_device, wideFilePath.c_str(), &texture)
  )) {
    throw std::runtime_error("loading texture file failed");
  }

  const auto nextIndex = m_textures.size();
  if (nextIndex > static_cast<u16>(std::numeric_limits<i16>::max())) {
    throw std::out_of_range("out of mesh slots");
  }
  i16 index = static_cast<i16>(nextIndex);

  TextureHandle handle(index, 0);

  m_textures.push_back({texture, handle});

  return handle;
}


void Renderer::Submit(const RenderCommand& command) {
  m_commandBuffers[0].AddCommand(command);
}


void Renderer::Submit(const RenderCommandBuffer&) {
  // TODO: implement additional command buffers
  throw std::runtime_error("not implemented");
}


void Renderer::SetViewProj(
  const math::Mat4f32& view, const math::Mat4f32& projection
) {
  if (projection.m34 < 0) {
    throw std::runtime_error("m34 can't be null in a projection matrix");
  }

  m_commandBuffers[0].SetView(view);
  m_commandBuffers[0].SetProjection(projection);
}

void Renderer::SetLights(const LightSetup& lights) {
  // TODO: set to max lights supported by this device
  constexpr DWORD MAX_LIGHTS = 8u;

  const auto& directionalLights = lights.GetDirectionalLights();
  if (directionalLights.size() > MAX_LIGHTS) {
    throw std::runtime_error("the renderer doesn't support that many lights");
  }

  DWORD lightIndex = 0u;
  for (const DirectionalLight& light : directionalLights) {
    D3DLIGHT9 d3dlight{};
    d3dlight.Type = D3DLIGHT_DIRECTIONAL;
    d3dlight.Direction = *reinterpret_cast<const D3DVECTOR*>(&light.direction);
    SetD3DColor(d3dlight.Diffuse, light.diffuseColor);
    SetD3DColor(d3dlight.Ambient, light.ambientColor);

    D3D9CALL(m_device->SetLight(lightIndex, &d3dlight));
    D3D9CALL(m_device->LightEnable(lightIndex, TRUE));
    lightIndex++;
  }

  // disable not used lights
  for (; lightIndex < MAX_LIGHTS; lightIndex++) {
    D3D9CALL(m_device->LightEnable(lightIndex, FALSE));
  }

  D3D9CALL(m_device->SetRenderState(
    D3DRS_AMBIENT, lights.GetGlobalAmbientColor().ToARGB()
  ));
}


void Renderer::SetClearColor(Color color) {
  m_clearColor = color.ToARGB();
}


// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void Renderer::Render() {
  D3D9CALL(m_device->Clear(
    0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_clearColor, 1.0f, 0u
  ));

  // TODO: should we make all rendering code dependant
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CALL(m_device->BeginScene());

  for (const RenderCommandBuffer& commandBuffer : m_commandBuffers) {
    D3D9CALL(m_device->SetTransform(
      D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&commandBuffer.GetView())
    ));
    D3D9CALL(m_device->SetTransform(
      D3DTS_PROJECTION,
      reinterpret_cast<const D3DMATRIX*>(&commandBuffer.GetProjection())
    ));

    RenderCommands(commandBuffer);
  }

  D3D9CALL(m_device->SetStreamSource(0u, nullptr, 0u, 0u));

  D3D9CALL(m_device->EndScene());

  // remove every command buffer except the default and clear the default buffer
  m_commandBuffers.resize(1);
  m_commandBuffers[0].Clear();
}


void Renderer::Present() {
  D3D9CALL(m_device->Present(nullptr, nullptr, nullptr, nullptr));
}


std::string_view Renderer::GetName() {
  return s_rendererName;
}


Mesh& Renderer::GenerateMeshSlot() {
  auto nextIndex = m_meshes.size();
  if (nextIndex > static_cast<u16>(std::numeric_limits<i16>::max())) {
    throw std::out_of_range("out of mesh slots");
  }
  i16 index = static_cast<i16>(nextIndex);
  Mesh& mesh = m_meshes.emplace_back();
  mesh.handle = MeshHandle(index, 0);

  return mesh;
}


Mesh& Renderer::GetFreeMeshSlot() {
  while (!m_freeMeshSlots.empty()) {
    i16 index = m_freeMeshSlots.back();
    m_freeMeshSlots.pop_back();
    Mesh& mesh = m_meshes.at(index);
    MeshHandle::GenT gen = mesh.handle.GetGen();
    if (gen == std::numeric_limits<MeshHandle::GenT>::max()) {
      // slot reached "end of life"
      continue;
    }

    mesh.handle = MeshHandle(index, gen);
    return mesh;
  }

  // no more allocated free mesh slots available. genereate a new one
  return GenerateMeshSlot();
}


Mesh& Renderer::GetMesh(MeshHandle meshHandle) {
  if (!meshHandle.IsValid()) {
    throw std::runtime_error("invalid handle");
  }

  // throws exception with invalid index
  Mesh& mesh = m_meshes.at(meshHandle.GetIndex());

  if (mesh.handle.GetGen() != meshHandle.GetGen()) {
    throw std::runtime_error("invalid handle");
  }

  return mesh;
}


void Renderer::RenderCommands(const RenderCommandBuffer& commands) {
  for (const RenderCommand& command : commands.GetCommands()) {
    D3DMATERIAL9 material{};
    SetD3DColor(material.Diffuse, command.diffuseColor);
    SetD3DColor(material.Ambient, command.ambientColor);
    SetD3DColor(material.Emissive, command.emissiveColor);
    D3D9CALL(m_device->SetMaterial(&material));

    if (command.texture.IsValid()) {
      const Texture& texture = m_textures.at(command.texture.GetIndex());
      D3D9CALL(m_device->SetTexture(0, texture.texture));
    } else {
      D3D9CALL(m_device->SetTexture(0, nullptr));
    }

    const Mesh& mesh = GetMesh(command.mesh);
    D3D9CALL(m_device->SetStreamSource(
      0u, mesh.vertexBuffer, 0u, mesh.vertexSize
    ));
    D3D9CALL(m_device->SetFVF(mesh.fvf));

    D3D9CALL(m_device->SetTransform(
      D3DTS_WORLDMATRIX(0), reinterpret_cast<const D3DMATRIX*>(&command.world)
    ));

    D3D9CALL(m_device->DrawPrimitive(mesh.primType, 0u, mesh.primCount));
  }
}

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt
