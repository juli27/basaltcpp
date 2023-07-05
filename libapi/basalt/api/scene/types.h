#pragma once

#include <basalt/api/shared/handle.h>

#include <entt/entity/fwd.hpp>

#include <memory>

namespace basalt {

using Entity = entt::handle;
using EntityId = entt::entity;
using EntityRegistry = entt::registry;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

struct SceneContext;

class System;
using SystemId = Handle<System>;
using SystemPtr = std::unique_ptr<System>;

struct SystemContext;

struct Transform;
struct LocalToWorld;
class TransformSystem;

} // namespace basalt
