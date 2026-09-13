#include <basalt/api/engine.h>

#include <basalt/api/gfx/context.h>

#include <utility>

using namespace std::string_literals;

namespace basalt {

auto Engine::gfx_context() const noexcept -> gfx::Context& {
  return *mGfxContext;
}

auto Engine::gfx_info() const noexcept -> gfx::Info const& {
  return mGfxContext->gfx_info();
}

auto Engine::create_gfx_resource_cache() const -> gfx::ResourceCachePtr {
  return mGfxContext->create_resource_cache();
}

auto Engine::root() const -> ViewPtr const& {
  return mRoot;
}

auto Engine::set_root(ViewPtr view) -> void {
  mRoot = std::move(view);
}

Engine::Engine(gfx::ContextPtr gfxContext) noexcept
  : mGfxContext{std::move(gfxContext)} {
}

} // namespace basalt
