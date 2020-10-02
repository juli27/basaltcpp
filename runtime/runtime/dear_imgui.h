#pragma once

#include "gfx/drawable.h"

#include <memory>

namespace basalt {

struct UpdateContext;

namespace gfx {

struct Device;

namespace ext {

struct DearImGuiRenderer;

}

} // namespace gfx

struct DearImGui final : gfx::Drawable {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui &&) -> DearImGui& = delete;

  void new_frame(const UpdateContext&) const;

  auto draw(gfx::Device&, Size2Du16 viewport) -> gfx::CommandList override;

  [[nodiscard]] auto clear_color() const -> std::optional<Color> override;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer {};
};

} // namespace basalt
