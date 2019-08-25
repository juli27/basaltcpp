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
  ~WindowResizedEvent() noexcept = default;

  auto operator=(const WindowResizedEvent&) noexcept -> WindowResizedEvent&
    = default;
  auto operator=(WindowResizedEvent&&) noexcept -> WindowResizedEvent&
    = default;

  math::Vec2i32 mNewSize;
};

constexpr WindowResizedEvent::WindowResizedEvent(
  const math::Vec2i32& newSize
) noexcept : mNewSize(newSize) {}

struct WindowMinimizedEvent : EventTyped<EventType::WindowMinimized> {
  constexpr WindowMinimizedEvent() noexcept = default;
  constexpr WindowMinimizedEvent(const WindowMinimizedEvent&) noexcept = default;
  constexpr WindowMinimizedEvent(WindowMinimizedEvent&&) noexcept = default;
  ~WindowMinimizedEvent() noexcept = default;

  auto operator=(const WindowMinimizedEvent&) noexcept -> WindowMinimizedEvent&
    = default;
  auto operator=(WindowMinimizedEvent&&) noexcept -> WindowMinimizedEvent&
    = default;
};

struct WindowRestoredEvent : EventTyped<EventType::WindowRestored> {
  constexpr WindowRestoredEvent() noexcept = default;
  constexpr WindowRestoredEvent(const WindowRestoredEvent&) noexcept = default;
  constexpr WindowRestoredEvent(WindowRestoredEvent&&) noexcept = default;
  ~WindowRestoredEvent() noexcept = default;

  auto operator=(const WindowRestoredEvent&) noexcept -> WindowRestoredEvent&
    = default;
  auto operator=(WindowRestoredEvent&&) noexcept -> WindowRestoredEvent&
    = default;
};

} // namespace basalt::platform

#endif // !BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
