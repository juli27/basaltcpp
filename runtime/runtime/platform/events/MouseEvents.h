#pragma once
#ifndef BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include "runtime/math/Vec2.h"
#include "runtime/shared/Types.h"

namespace basalt::platform {

struct MouseMovedEvent final : EventTyped<EventType::MouseMoved> {
  math::Vec2i32 pos {};

  constexpr explicit MouseMovedEvent(const math::Vec2i32& pos) noexcept
    : pos {pos} {
  }
  constexpr MouseMovedEvent(const MouseMovedEvent&) noexcept = default;
  constexpr MouseMovedEvent(MouseMovedEvent&&) noexcept = default;

  ~MouseMovedEvent() noexcept = default;

  auto operator=(const MouseMovedEvent&) -> MouseMovedEvent& = default;
  auto operator=(MouseMovedEvent&&) -> MouseMovedEvent& = default;
};

struct MouseWheelScrolledEvent final
  : EventTyped<EventType::MouseWheelScrolled> {
  f32 offset {};

  constexpr explicit MouseWheelScrolledEvent(const f32 offset) noexcept
    : offset {offset} {
  }

  constexpr MouseWheelScrolledEvent(const MouseWheelScrolledEvent&) noexcept
  = default;
  constexpr MouseWheelScrolledEvent(MouseWheelScrolledEvent&&) noexcept
  = default;

  ~MouseWheelScrolledEvent() noexcept = default;

  auto operator=(
    const MouseWheelScrolledEvent&) -> MouseWheelScrolledEvent& = default;
  auto operator=(
    MouseWheelScrolledEvent&&) -> MouseWheelScrolledEvent& = default;
};

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
