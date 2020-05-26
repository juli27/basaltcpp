#pragma once
#ifndef BASALT_APP_D3D9_CONTEXT_FACTORY_H
#define BASALT_APP_D3D9_CONTEXT_FACTORY_H

#include "d3d9_custom.h"
#include "types.h"

#include <wrl/client.h>

#include <memory>
#include <optional>

namespace basalt::gfx::backend {

struct D3D9ContextFactory;
using D3D9ContextFactoryPtr = std::unique_ptr<D3D9ContextFactory>;

struct D3D9GfxContext;

struct D3D9ContextFactory final {
  D3D9ContextFactory() = delete;
  explicit D3D9ContextFactory(Microsoft::WRL::ComPtr<IDirect3D9> factory);

  D3D9ContextFactory(const D3D9ContextFactory& other) = delete;
  D3D9ContextFactory(D3D9ContextFactory&& other) = delete;

  ~D3D9ContextFactory() = default;

  auto operator=(
    const D3D9ContextFactory& other) -> D3D9ContextFactory& = delete;
  auto operator=(D3D9ContextFactory&& other) -> D3D9ContextFactory& = delete;

  [[nodiscard]]
  auto adapter_info() const -> const AdapterInfo&;

  auto create_context(HWND window) const -> std::unique_ptr<D3D9GfxContext>;

private:
  Microsoft::WRL::ComPtr<IDirect3D9> mFactory;
  AdapterInfo mAdapterInfo {};

public:
  static auto create() -> std::optional<D3D9ContextFactoryPtr>;
};

} // namespace basalt::gfx::backend

#endif // BASALT_APP_D3D9_CONTEXT_FACTORY_H
