#pragma once

#include <basalt/api/layer.h>

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <memory>

namespace basalt {

struct DearImGui final
  : gfx::Drawable
  , Layer {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  auto draw(gfx::ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<gfx::CommandList, RectangleU16> override;

  void tick(Engine&) override;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  auto do_handle_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
