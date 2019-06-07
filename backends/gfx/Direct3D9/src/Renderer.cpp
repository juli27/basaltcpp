#include <basalt/gfx/backend/d3d9/Renderer.h>

#include <cmath>

#include <stdexcept>
#include <vector>

#include <basalt/common/Asserts.h>
#include <basalt/common/Color.h>
#include <basalt/gfx/backend/d3d9/D3D9Header.h>
#include <basalt/gfx/backend/d3d9/Util.h>

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {
namespace {

constexpr std::string_view s_rendererName = "Direct3D 9 fixed function";


DWORD GetFVF(const VertexLayout& layout) {
  DWORD fvf = 0u;

  for (const VertexElement& element : layout.m_elements) {
    switch (element.usage) {
      case VertexElementUsage::POSITION:
        if (element.type != VertexElementType::F32_3) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_XYZ;
        break;

      case VertexElementUsage::NORMAL:
        if (element.type != VertexElementType::F32_3) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_NORMAL;
        break;

      case VertexElementUsage::COLOR_DIFFUSE:
        if (element.type != VertexElementType::U32_1) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_DIFFUSE;
        break;

      case VertexElementUsage::TEXTURE_COORDS:
        if (element.type != VertexElementType::F32_2) {
          throw std::runtime_error("vertex layout not supported");
        }
        fvf |= D3DFVF_TEX1;
        break;

      default:
        throw std::runtime_error("vertex layout not supported");
        break;
    }
  }

  return fvf;
}


void SetPrimitiveInfo(Mesh& mesh, const VertexData& vertices) {
  switch (vertices.primitiveType) {
    case PrimitiveType::POINT_LIST:
      mesh.primType = D3DPT_POINTLIST;
      mesh.primCount = vertices.numVertices;
      break;

    case PrimitiveType::LINE_LIST:
      mesh.primType = D3DPT_LINELIST;
      BS_ASSERT(
        vertices.numVertices % 2 == 0,
        "Wrong amount of vertices for PrimitiveType::LINE_LIST"
      );
      mesh.primCount = vertices.numVertices / 2;
      break;

    case PrimitiveType::LINE_STRIP:
      mesh.primType = D3DPT_LINESTRIP;
      mesh.primCount = vertices.numVertices - 1;
      break;

    case PrimitiveType::TRIANGLE_LIST:
      mesh.primType = D3DPT_TRIANGLELIST;
      BS_ASSERT(
        vertices.numVertices % 3 == 0,
        "Wrong amount of vertices for PrimitiveType::TRIANGLE_LIST"
      );
      mesh.primCount = vertices.numVertices / 3;
      break;

    case PrimitiveType::TRIANGLE_STRIP:
      mesh.primType = D3DPT_TRIANGLESTRIP;
      mesh.primCount = vertices.numVertices - 2;
      break;

    case PrimitiveType::TRIANGLE_FAN:
      mesh.primType = D3DPT_TRIANGLEFAN;
      mesh.primCount = vertices.numVertices - 2;
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


// TODO: should use 32 byte vertex?
MeshHandle Renderer::AddMesh(const VertexData& vertexData) {
  const DWORD fvf = GetFVF(vertexData.layout);
  const UINT vertexSize = D3DXGetFVFVertexSize(fvf);
  const UINT bufferSize = vertexSize * vertexData.numVertices;

  // TODO: specify write only and correct pool
  IDirect3DVertexBuffer9* vertexBuffer = nullptr;
  D3D9CALL(m_device->CreateVertexBuffer(
      bufferSize, 0u, fvf, D3DPOOL_DEFAULT, &vertexBuffer, nullptr
  ));

  void* vertexBufferData = nullptr;
  if (SUCCEEDED(vertexBuffer->Lock(0u, 0u, &vertexBufferData, 0u))) {
    std::memcpy(vertexBufferData, vertexData.data, bufferSize);
    D3D9CALL(vertexBuffer->Unlock());
  } else {
    BS_ERROR("Failed to lock vertex buffer");
  }

  const auto nextIndex = m_meshes.size();
  if (nextIndex > static_cast<u16>(std::numeric_limits<i16>::max())) {
    throw std::out_of_range("out of mesh slots");
  }
  i16 index = static_cast<i16>(nextIndex);

  Mesh mesh{vertexBuffer, MeshHandle(index, 0), fvf, vertexSize};
  SetPrimitiveInfo(mesh, vertexData);
  m_meshes.push_back(mesh);

  return mesh.handle;
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

  m_textures.push_back({handle, texture});

  return handle;
}


void Renderer::Submit(const RenderCommand& command) {
  m_commandQueue.push_back(command);
}

void Renderer::SetViewProj(
  const math::Mat4f32& view, const math::Mat4f32& projection
) {
  if (projection.m34 < 0) {
    throw std::runtime_error("m34 can't be null in a projection matrix");
  }

  D3D9CALL(m_device->SetTransform(
    D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view)
  ));
  D3D9CALL(m_device->SetTransform(
    D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&projection)
  ));
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

// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void Renderer::Render() {
  // setup render states
  D3D9CALL(m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

  D3D9CALL(m_device->Clear(
    0u, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 63), 1.0f, 0u
  ));

  // TODO: should we make all rendering code dependant
  // on the success of BeginScene? -> Log error and/or throw exception
  D3D9CALL(m_device->BeginScene());

  for (const RenderCommand& command : m_commandQueue) {
    const Mesh& mesh = m_meshes.at(command.mesh.GetIndex());

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

    D3D9CALL(m_device->SetStreamSource(
      0, mesh.vertexBuffer, 0, mesh.vertexSize
    ));
    D3D9CALL(m_device->SetFVF(mesh.fvf));

    D3D9CALL(m_device->SetTransform(
      D3DTS_WORLDMATRIX(0), reinterpret_cast<const D3DMATRIX*>(&command.world)
    ));

    m_device->DrawPrimitive(mesh.primType, 0, mesh.primCount);
  }

  m_device->EndScene();

  // reset to default
  m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

  m_commandQueue.clear();
}


void Renderer::Present() {
  m_device->Present(nullptr, nullptr, nullptr, nullptr);
}


std::string_view Renderer::GetName() {
  return s_rendererName;
}

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt
