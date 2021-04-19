#pragma once

#include <basalt/api/input_layer.h>

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <memory>

namespace basalt {

struct DearImGui final
  : gfx::Drawable
  , InputLayer {
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

  auto do_handle_input(const InputEvent&) -> InputEventHandled override;
};

} // namespace basalt
