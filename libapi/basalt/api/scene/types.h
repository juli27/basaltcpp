#pragma once

#include <basalt/api/shared/handle.h>

#include <entt/entity/fwd.hpp>

#include <memory>
#include <string>

namespace basalt {

using Entity = entt::handle;
using EntityId = entt::entity;
using EntityRegistry = entt::registry;

struct EntityName {
  std::string value;
};

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

struct SceneContext;

class System;
BASALT_DEFINE_HANDLE(SystemId);
using SystemPtr = std::unique_ptr<System>;

struct Transform;
struct LocalToWorld;
class TransformSystem;
class ParentSystem;

} // namespace basalt

template <>
struct std::hash<basalt::SystemId> {
  auto operator()(basalt::SystemId const& handle) const -> std::size_t {
    return std::hash<typename basalt::SystemId::ValueType>{}(handle.value());
  }
};
