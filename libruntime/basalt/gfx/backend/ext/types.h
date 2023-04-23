#pragma once

#include <memory>

namespace basalt::gfx::ext {

struct Extension;
using ExtensionPtr = std::shared_ptr<Extension>;

enum class ExtensionId {
  DearImGuiRenderer,
  XModelSupport,
};

class DearImGuiRenderer;
class XModelSupport;

} // namespace basalt::gfx::ext
