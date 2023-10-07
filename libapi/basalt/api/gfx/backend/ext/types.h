#pragma once

#include <basalt/api/shared/handle.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <string>
#include <variant>

namespace basalt::gfx::ext {

class DeviceExtension;
using DeviceExtensionPtr = std::shared_ptr<DeviceExtension>;

enum class DeviceExtensionId : u8;

namespace detail {

struct XMeshTag;
struct XModelTag;
struct EffectTechniqueTag;

} // namespace detail

using XMesh = Handle<detail::XMeshTag>;
using XModel = Handle<detail::XModelTag>;

class XMeshCommandEncoder;

class Effect;
using EffectId = Handle<Effect>;
using EffectTechniqueHandle = Handle<detail::EffectTechniqueTag>;
using CompileResult = std::variant<std::string, EffectId>;

} // namespace basalt::gfx::ext
