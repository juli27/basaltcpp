#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <memory>

namespace basalt {

struct DearImGui final
  : gfx::Drawable
  , View {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  auto on_draw(gfx::ResourceCache&, Size2Du16 viewport,
               const RectangleU16& clip)
    -> std::tuple<gfx::CommandList, RectangleU16> override;

  void on_tick(Engine&) override;

  auto on_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
