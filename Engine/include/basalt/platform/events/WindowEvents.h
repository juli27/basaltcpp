#pragma once
#ifndef BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
#define BS_PLATFORM_EVENTS_WINDOW_EVENTS_H

#include "Event.h"

#include <basalt/math/Vec2.h>

namespace basalt::platform {


struct WindowResizedEvent : EventTyped<EventType::WindowResized> {
  constexpr explicit WindowResizedEvent(const math::Vec2i32& newSize) noexcept;
  constexpr WindowResizedEvent(const WindowResizedEvent&) noexcept = default;
  constexpr WindowResizedEvent(WindowResizedEvent&&) noexcept = default;
  inline ~WindowResizedEvent() noexcept = default;

  inline auto operator=(const WindowResizedEvent&) noexcept
    -> WindowResizedEvent& = default;
  inline auto operator=(WindowResizedEvent&&) noexcept -> WindowResizedEvent&
    = default;

  math::Vec2i32 mNewSize;
};


constexpr WindowResizedEvent::WindowResizedEvent(
  const math::Vec2i32& newSize
) noexcept : mNewSize(newSize) {}

} // namespace basalt::platform

#endif // !BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
