#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <memory>

namespace basalt {

struct DearImGui final : View {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  auto on_draw(const DrawContext&) -> gfx::CommandList override;

  void on_tick(Engine&) override;

  auto on_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
