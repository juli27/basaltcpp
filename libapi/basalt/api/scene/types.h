#pragma once

#include <basalt/api/shared/handle.h>

#include <memory>

namespace basalt {

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

struct SceneContext;

class System;
using SystemId = Handle<System>;
using SystemPtr = std::unique_ptr<System>;

struct SystemContext;

struct Transform;

} // namespace basalt
