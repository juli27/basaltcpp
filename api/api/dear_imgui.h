#pragma once

#include "gfx/drawable.h"

#include "types.h"
#include "gfx/backend/ext/types.h"

#include <memory>

namespace basalt {

struct DearImGui final : gfx::Drawable {
  explicit DearImGui(gfx::Device&);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui() override;

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui &&) -> DearImGui& = delete;

  void new_frame(const UpdateContext&) const;

  auto draw(gfx::ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<gfx::CommandList, RectangleU16> override;

private:
  std::shared_ptr<gfx::ext::DearImGuiRenderer> mRenderer;
};

} // namespace basalt
