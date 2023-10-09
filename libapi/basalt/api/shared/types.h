#pragma once

#include <basalt/api/base/types.h>

#include <entt/core/hashed_string.hpp>

#include <chrono>
#include <memory>

namespace basalt {

struct Color;
class Config;

template <typename T, typename Handle>
class HandlePool;

template <typename T>
class Size2D;
using Size2Du16 = Size2D<u16>;

using SecondsF32 = std::chrono::duration<f32>;

// SERIALIZED
enum class WindowMode : u8 {
  Windowed = 0,
  Fullscreen = 1,
  FullscreenExclusive = 2,
};

constexpr auto WINDOW_MODE_COUNT = 3;

constexpr auto to_window_mode(i32 const num) noexcept -> WindowMode {
  // TODO: BASALT_ASSERT(num < WINDOW_MODE_COUNT);

  if (num >= WINDOW_MODE_COUNT || num < 0) {
    return WindowMode::Windowed;
  }

  return WindowMode{static_cast<u8>(num)};
}

using Resource = entt::hashed_string;
using ResourceId = entt::hashed_string::hash_type;

class ResourceRegistry;
using ResourceRegistryPtr = std::shared_ptr<ResourceRegistry>;

} // namespace basalt
