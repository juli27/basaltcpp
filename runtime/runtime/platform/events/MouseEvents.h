#pragma once
#ifndef BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include "runtime/shared/Types.h"

namespace basalt::platform {

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
