#pragma once

#include "api/shared/handle.h"

#include <memory>

namespace basalt::gfx::ext {

struct Extension;
using ExtensionPtr = std::shared_ptr<Extension>;

enum class ExtensionId { DearImGuiRenderer, XModelSupport };

struct DearImGuiRenderer;
struct XModelSupport;

namespace detail {

struct XModelTag;

} // namespace detail

using XModel = Handle<detail::XModelTag>;

} // namespace basalt::gfx::ext
