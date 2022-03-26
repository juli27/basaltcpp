#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <memory>

namespace basalt {

struct DearImGui final : View {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  auto new_frame(Engine&, Size2Du16 displaySize) const -> void;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  void on_draw(const DrawContext&) override;

  auto on_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
