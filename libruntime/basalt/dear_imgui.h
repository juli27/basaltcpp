#pragma once

#include <basalt/types.h>

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <memory>

namespace basalt {

class DearImGui final : public View {
public:
  static auto create(gfx::Device&, void* rawWindowHandle) -> DearImGuiPtr;

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  auto new_frame(Engine&, Size2Du16 displaySize) const -> void;

  // do not call directly. use the static create function instead
  explicit DearImGui(std::shared_ptr<gfx::ext::DearImGuiRenderer>);

private:
  // can be null
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  auto on_draw(const DrawContext&) -> void override;

  auto on_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
