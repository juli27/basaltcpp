#pragma once

#include <basalt/api/base/types.h>

#include <entt/core/hashed_string.hpp>

#include <chrono>
#include <memory>

namespace basalt {

struct Color;
struct Config;

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

constexpr i32 WINDOW_MODE_COUNT = 3u;

constexpr auto to_window_mode(const i32 num) noexcept -> WindowMode {
  // TODO: BASALT_ASSERT(num < WINDOW_MODE_COUNT);

  if (num >= WINDOW_MODE_COUNT || num < 0) {
    return WindowMode::Windowed;
  }

  return WindowMode {static_cast<u8>(num)};
}

using Resource = entt::hashed_string;
using ResourceId = entt::hashed_string::hash_type;

struct ResourceRegistry;
using ResourceRegistryPtr = std::shared_ptr<ResourceRegistry>;

} // namespace basalt
