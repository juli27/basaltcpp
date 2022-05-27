#pragma once

#include <basalt/api/shared/handle.h>

#include <memory>

namespace basalt::gfx::ext {

struct Extension;
using ExtensionPtr = std::shared_ptr<Extension>;

enum class ExtensionId {
  DearImGuiRenderer,
  XModelSupport,
};

struct DearImGuiRenderer;

struct XModelSupport;
namespace detail {

struct XMeshTag;

} // namespace detail
using XMesh = Handle<detail::XMeshTag>;
class XModelData;
using XModel = Handle<XModelData>;

} // namespace basalt::gfx::ext
