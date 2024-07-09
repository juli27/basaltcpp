#pragma once

#include <basalt/types.h>

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

#include <memory>

namespace basalt {

class DearImGui final : public View {
public:
  [[nodiscard]]
  static auto create(gfx::Context&) -> DearImGuiPtr;

  // do not call directly. use the static create function instead
  explicit DearImGui(std::shared_ptr<gfx::ext::DearImGuiRenderer>);

  DearImGui(DearImGui const&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() noexcept override;

  auto operator=(DearImGui const&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  auto new_frame(UpdateContext const&) const -> void;

private:
  // can be null
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;

  auto on_update(UpdateContext&) -> void override;

  auto on_input(InputEvent const&) -> InputEventHandled override;
};

} // namespace basalt
