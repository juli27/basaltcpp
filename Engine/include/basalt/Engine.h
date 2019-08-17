#pragma once
#ifndef BS_ENGINE_H
#define BS_ENGINE_H

#include <memory>

#include "Scene.h"
#include "common/Types.h"

namespace basalt {


void Run();


auto GetDeltaTime() -> f64;


void SetCurrentScene(const std::shared_ptr<Scene>& scene);

} // namespace basalt

#endif // !BS_ENGINE_H
