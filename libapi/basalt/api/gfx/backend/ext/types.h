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

BASALT_DEFINE_HANDLE(XMeshHandle);

class XMeshCommandEncoder;
struct XModelData;

class Effect;
BASALT_DEFINE_HANDLE(EffectId);
BASALT_DEFINE_HANDLE(EffectTechniqueHandle);
using CompileResult = std::variant<std::string, EffectId>;

} // namespace basalt::gfx::ext
