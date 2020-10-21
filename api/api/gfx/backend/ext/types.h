#pragma once

#include "api/shared/handle.h"

#include <memory>

namespace basalt::gfx::ext {

struct Extension;
using ExtensionPtr = std::shared_ptr<Extension>;

enum class ExtensionId { DearImGuiRenderer, XModelSupport };

struct DearImGuiRenderer;
struct XModelSupport;

enum class XModel : u32 {};
using XModelHandle = Handle<XModel>;

} // namespace basalt::gfx::ext
