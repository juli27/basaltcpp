#pragma once

#include "basalt/api/base/types.h"

#include <chrono>

namespace basalt {

struct Color;
class Config;

template <typename T, typename Handle>
class HandlePool;

template <typename Handle, typename Deleter>
class UniqueHandle;

template <typename T>
class Size2D;
using Size2Du16 = Size2D<u16>;

using SecondsF32 = std::chrono::duration<f32>;

} // namespace basalt
