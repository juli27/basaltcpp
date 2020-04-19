#pragma once
#ifndef BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H
#define BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H

#include "Event.h"

#include "runtime/shared/Size2D.h"

namespace basalt::platform {

struct WindowCloseRequestEvent : EventTyped<EventType::WindowCloseRequest> {
  constexpr WindowCloseRequestEvent() noexcept = default;
  constexpr WindowCloseRequestEvent(const WindowCloseRequestEvent&) noexcept = default;
  constexpr WindowCloseRequestEvent(WindowCloseRequestEvent&&) noexcept = default;
  ~WindowCloseRequestEvent() noexcept = default;

  auto operator=(const WindowCloseRequestEvent&) noexcept -> WindowCloseRequestEvent&
    = default;
  auto operator=(WindowCloseRequestEvent&&) noexcept -> WindowCloseRequestEvent&
    = default;
};


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

#endif // !BASALT_PLATFORM_EVENTS_WINDOW_EVENTS_H