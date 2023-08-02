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
using SystemId = Handle<System>;
using SystemPtr = std::unique_ptr<System>;

struct Transform;
struct LocalToWorld;
class TransformSystem;
class ParentSystem;

} // namespace basalt
