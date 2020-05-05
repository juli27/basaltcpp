#pragma once
#ifndef BASALT_PLATFORM_WIN32_D3D9_CONTEXT_FACTORY_H
#define BASALT_PLATFORM_WIN32_D3D9_CONTEXT_FACTORY_H

#include "runtime/platform/win32/D3D9GfxContext.h"
#include "runtime/shared/win32/D3D9Header.h"
#include "runtime/shared/win32/Win32APIHeader.h"

#include <wrl/client.h>

#include <memory>
#include <optional>

namespace basalt::platform {

struct D3D9ContextFactory final {
  D3D9ContextFactory() = delete;
  explicit D3D9ContextFactory(Microsoft::WRL::ComPtr<IDirect3D9> factory);

  D3D9ContextFactory(const D3D9ContextFactory& other) = delete;
  D3D9ContextFactory(D3D9ContextFactory&& other) = delete;

  ~D3D9ContextFactory() = default;

  auto operator=(
    const D3D9ContextFactory& other
  ) -> D3D9ContextFactory& = delete;
  auto operator=(D3D9ContextFactory&& other) -> D3D9ContextFactory& = delete;

  auto create_context(HWND window) const -> std::unique_ptr<D3D9GfxContext>;

private:
  Microsoft::WRL::ComPtr<IDirect3D9> mFactory;

public:
  static auto create() -> std::optional<std::unique_ptr<D3D9ContextFactory>>;
};

} // namespace basalt::platform

#endif // BASALT_PLATFORM_WIN32_D3D9_CONTEXT_FACTORY_H
