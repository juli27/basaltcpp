#include <basalt/gfx/backend/d3d9/Renderer.h>

#include <basalt/common/Asserts.h>
#include <basalt/common/Color.h>
#include <basalt/gfx/backend/d3d9/Util.h>
#include <basalt/platform/Platform.h>
#include <basalt/platform/events/WindowEvents.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>

#include <limits>
#include <stdexcept>
#include <string>

#include <cstring> // for memcpy

namespace basalt::gfx::backend::d3d9 {
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
  Mesh& mesh, const PrimitiveType primitiveType, const i32 numVtx
) {
  switch (primitiveType) {
    case PrimitiveType::PointList:
      mesh.primType = D3DPT_POINTLIST;
      mesh.primCount = numVtx;
      break;

    case PrimitiveType::LineList:
      mesh.primType = D3DPT_LINELIST;
      BS_ASSERT(
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
      BS_ASSERT(
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
  d3dColor.r = color.get_red() / 255.0f;
  d3dColor.g = color.get_green() / 255.0f;
  d3dColor.b = color.get_blue() / 255.0f;
  d3dColor.a = color.get_alpha() / 255.0f;
}

auto create_wide_from_utf8(const std::string_view src) noexcept
-> std::wstring {
  // TODO: noexcept allocator and heap memory pool for strings

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(std::numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: string to convert is too large";
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, nullptr, 0
  );

  if (dstSize == 0) {
    return L"MultiByteToWideChar returned 0";
  }

  std::wstring dst(dstSize, L'\0');
  dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, dst.data(), static_cast<int>(dst.size())
  );

  if (dstSize == 0) {
    return L"MultiByteToWideChar returned 0";
  }

  return dst;
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

      BS_INFO("resized d3d9 back buffer");
    });
  });
}

Renderer::~Renderer() {
  ImGui_ImplDX9_Shutdown();

  mTextures.for_each([](Texture& texture){
    texture.texture->Release();
  });
  mMeshes.for_each([](Mesh& mesh){
    mesh.vertexBuffer->Release();
  });

  mDevice->Release();
}

/*
 * Stores the vertex data into a new static vertex buffer in the managed pool.
 */
auto Renderer::add_mesh(
  void* data, const i32 numVertices, const VertexLayout& layout,
  const PrimitiveType primitiveType
) -> MeshHandle {
  BS_ASSERT_ARG_NOT_NULL(data);
  BS_ASSERT(numVertices > 0, "numVertices must be > 0");
  BS_ASSERT(!layout.get_elements().empty(), "must specify a vertex layout");

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
    BS_ERROR("Failed to lock vertex buffer");
  }

  const auto [meshHandle, mesh] = mMeshes.allocate();
  mesh.vertexBuffer = vertexBuffer;
  mesh.fvf = fvf;
  mesh.vertexSize = vertexSize;
  fill_primitive_info(mesh, primitiveType, numVertices);

  return meshHandle;
}

void Renderer::remove_mesh(const MeshHandle meshHandle) {
  auto& mesh = mMeshes.get(meshHandle);

  mesh.vertexBuffer->Release();
  mesh.vertexBuffer = nullptr;

  mMeshes.deallocate(meshHandle);
}

auto Renderer::add_texture(const std::string_view filePath) -> TextureHandle {
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

void Renderer::remove_texture(const TextureHandle textureHandle) {
  auto& texture = mTextures.get(textureHandle);

  texture.texture->Release();
  texture.texture = nullptr;

  mTextures.deallocate(textureHandle);
}

void Renderer::submit(const RenderCommand& command) {
  mCommandBuffer.add_command(command);
}

void Renderer::set_view_proj(
  const math::Mat4f32& view, const math::Mat4f32& projection
) {
  BS_ASSERT(projection.m34 >= 0,
    "m34 can't be negative in a projection matrix");

  mCommandBuffer.set_view(view);
  mCommandBuffer.set_projection(projection);
}

void Renderer::set_lights(const LightSetup& lights) {
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

void Renderer::set_clear_color(const Color color) {
  mClearColor = color.to_argb();
}

// TODO: shading mode
// TODO: lost device (resource location: Default, Managed, kept in RAM by us)
void Renderer::render() {
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
    D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.get_view())
  ));
  D3D9CALL(mDevice->SetTransform(
    D3DTS_PROJECTION,
    reinterpret_cast<const D3DMATRIX*>(&mCommandBuffer.get_projection())
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

auto Renderer::get_name() -> std::string_view {
  return RENDERER_NAME;
}


void Renderer::new_gui_frame() {
  ImGui_ImplDX9_NewFrame();
}

void Renderer::render_commands(const RenderCommandBuffer& commands) {
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

} // namespace basalt::gfx::backend::d3d9
