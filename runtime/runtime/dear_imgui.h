#pragma once

#include "gfx/drawable.h"

namespace basalt {

struct UpdateContext;

namespace gfx {

struct Device;

} // namespace gfx

struct DearImGui final : gfx::Drawable {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  void new_frame(const UpdateContext&) const;

  auto draw(gfx::Device&, Size2Du16 viewport) -> gfx::CommandList override;

  [[nodiscard]]
  auto clear_color() const -> const Color& override;

private:
  gfx::Device& mRenderer;
};

} // namespace basalt
