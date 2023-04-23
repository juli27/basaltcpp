#pragma once

#include <basalt/api/shared/handle.h>

namespace basalt::gfx::ext {

namespace detail {

struct XMeshTag;

} // namespace detail
using XMesh = Handle<detail::XMeshTag>;
class XModelData;
using XModel = Handle<XModelData>;

} // namespace basalt::gfx::ext
