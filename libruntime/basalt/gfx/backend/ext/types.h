#pragma once

#include <memory>

namespace basalt::gfx::ext {

struct Extension;
using ExtensionPtr = std::shared_ptr<Extension>;

enum class ExtensionId {
  DearImGuiRenderer,
  XModelSupport,
  Texture3DSupport,
};

class DearImGuiRenderer;
class XModelSupport;
struct XModelData;
class Texture3DSupport;

} // namespace basalt::gfx::ext
