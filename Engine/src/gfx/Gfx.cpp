#include "pch.h"

#include <basalt/gfx/Gfx.h>

#include <basalt/Log.h>

#include <basalt/gfx/backend/Factory.h>
#include <basalt/gfx/backend/IRenderer.h>

namespace basalt {
namespace gfx {
namespace {


backend::IRenderer* s_renderer;

} // namespace


void Init() {
  s_renderer = backend::CreateRenderer();
  BS_INFO("gfx backend: {}", s_renderer->GetName());
}


void Shutdown() {
  if (s_renderer) {
    delete s_renderer;
    s_renderer = nullptr;
  }

  gfx::backend::Shutdown();
}


void Render() {
  s_renderer->Render();
}


void Present() {
  s_renderer->Present();
}


backend::IRenderer* GetRenderer() {
  return s_renderer;
}

} // namespace gfx
} // namespace basalt
