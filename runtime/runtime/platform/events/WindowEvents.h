#pragma once
#ifndef BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H
#define BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H

#include "Event.h"

#include "runtime/shared/Size2D.h"

namespace basalt::platform {

struct WindowResizedEvent : EventTyped<EventType::WindowResized> {
  constexpr explicit WindowResizedEvent(const Size2Du16& newSize) noexcept;
  constexpr WindowResizedEvent(const WindowResizedEvent&) noexcept = default;
  constexpr WindowResizedEvent(WindowResizedEvent&&) noexcept = default;
  ~WindowResizedEvent() noexcept = default;

  auto operator=(const WindowResizedEvent&) noexcept -> WindowResizedEvent&
    = default;
  auto operator=(WindowResizedEvent&&) noexcept -> WindowResizedEvent&
    = default;

  Size2Du16 mNewSize;
};

constexpr WindowResizedEvent::WindowResizedEvent(
  const Size2Du16& newSize
) noexcept : mNewSize(newSize) {}

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H
