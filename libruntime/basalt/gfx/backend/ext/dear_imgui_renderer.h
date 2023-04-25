#pragma once

#include <basalt/gfx/backend/ext/extension.h>
#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx::ext {

class DearImGuiRenderer : public ExtensionT<ExtensionId::DearImGuiRenderer> {
public:
  virtual auto init() -> void = 0;
  virtual auto shutdown() -> void = 0;
  virtual auto new_frame() -> void = 0;
};

class DearImGuiCommandEncoder final {
public:
  static auto render_dear_imgui(CommandList&) -> void;
};

} // namespace basalt::gfx::ext
