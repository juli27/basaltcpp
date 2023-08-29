#pragma once

// transitive include
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx::ext {

enum class DeviceExtensionId : u8 {
  DearImGuiRenderer,
  XModelSupport,
  Texture3DSupport,
};

class DearImGuiRenderer;
class XModelSupport;
struct XModelData;
class Texture3DSupport;

} // namespace basalt::gfx::ext
