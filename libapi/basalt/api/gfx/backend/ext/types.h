#pragma once

#include <basalt/api/shared/handle.h>

#include <basalt/api/base/types.h>

#include <memory>

namespace basalt::gfx::ext {

class DeviceExtension;
using DeviceExtensionPtr = std::shared_ptr<DeviceExtension>;

enum class DeviceExtensionId : u8;

namespace detail {

struct XMeshTag;
struct XModelTag;

} // namespace detail

using XMesh = Handle<detail::XMeshTag>;
using XModel = Handle<detail::XModelTag>;

class XMeshCommandEncoder;

} // namespace basalt::gfx::ext
