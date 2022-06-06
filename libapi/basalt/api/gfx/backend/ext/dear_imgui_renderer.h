#pragma once

#include <basalt/api/gfx/backend/ext/extension.h>

namespace basalt::gfx::ext {

struct DearImGuiRenderer : ExtensionT<ExtensionId::DearImGuiRenderer> {
  virtual void init() = 0;
  virtual void shutdown() = 0;
  virtual void new_frame() = 0;
};

} // namespace basalt::gfx::ext
