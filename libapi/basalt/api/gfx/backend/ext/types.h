#pragma once

#include <basalt/api/shared/handle.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace basalt::gfx::ext {

class DeviceExtension;
using DeviceExtensionPtr = std::shared_ptr<DeviceExtension>;

enum class DeviceExtensionId : u8;

using DeviceExtensions =
  std::unordered_map<DeviceExtensionId, DeviceExtensionPtr>;

namespace detail {

struct XMeshTag;
struct XModelTag;
struct EffectTechniqueTag;

} // namespace detail

using XMeshHandle = Handle<detail::XMeshTag>;
using XModelHandle = Handle<detail::XModelTag>;

class XMeshCommandEncoder;

class Effect;
using EffectId = Handle<Effect>;
using EffectTechniqueHandle = Handle<detail::EffectTechniqueTag>;
using CompileResult = std::variant<std::string, EffectId>;

} // namespace basalt::gfx::ext
