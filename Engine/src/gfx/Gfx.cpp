#include "pch.h"

#include <basalt/gfx/Gfx.h>

#include <basalt/Log.h>

#include <basalt/gfx/backend/IRenderer.h>

namespace basalt::gfx {
namespace {


backend::IRenderer* s_renderer;

} // namespace


void Init() {
  s_renderer = backend::IRenderer::Create(backend::RendererType::Default);
  BS_INFO("gfx backend: {}", s_renderer->GetName());
}


void Shutdown() {
  if (s_renderer) {
    delete s_renderer;
    s_renderer = nullptr;
  }
}


void Render() {
  s_renderer->Render();
}


void Present() {
  s_renderer->Present();
}


auto GetRenderer() -> backend::IRenderer* {
  return s_renderer;
}

} // namespace basalt::gfx
