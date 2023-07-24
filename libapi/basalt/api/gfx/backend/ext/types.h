#pragma once

#include <basalt/api/shared/handle.h>

namespace basalt::gfx::ext {

namespace detail {

struct XMeshTag;
struct XModelTag;

} // namespace detail

using XMesh = Handle<detail::XMeshTag>;
using XModel = Handle<detail::XModelTag>;

class XMeshCommandEncoder;

} // namespace basalt::gfx::ext
