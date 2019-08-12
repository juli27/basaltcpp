#pragma once
#ifndef BS_ENGINE_H
#define BS_ENGINE_H

#include "common/Types.h"

namespace basalt {

void Run();

auto GetDeltaTime() -> f64;

} // namespace basalt

#endif // !BS_ENGINE_H
