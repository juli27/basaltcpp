#pragma once

#include "api/base/types.h"

#include <entt/core/hashed_string.hpp>

#include <memory>

namespace basalt {

struct Color;
struct Config;

template <typename T>
struct Size2D;
using Size2Du16 = Size2D<u16>;

enum class WindowMode : u8 {
  Windowed,
  Fullscreen,
  FullscreenExclusive,
};

using Resource = entt::hashed_string;
using ResourceId = entt::hashed_string::hash_type;

struct ResourceRegistry;
using ResourceRegistryPtr = std::shared_ptr<ResourceRegistry>;

} // namespace basalt
