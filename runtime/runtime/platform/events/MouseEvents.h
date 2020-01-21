#pragma once
#ifndef BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include "runtime/math/Vec2.h"
#include "runtime/shared/Types.h"

namespace basalt::platform {

enum class MouseButton : i8 {
  Unknown = 0,
  Left, Right, Middle, Button4, Button5,

  NumberOfButtons
};


struct MouseMovedEvent : EventTyped<EventType::MouseMoved> {
  constexpr explicit MouseMovedEvent(const math::Vec2i32& pos) noexcept;
  constexpr MouseMovedEvent(const MouseMovedEvent&) noexcept = default;
  constexpr MouseMovedEvent(MouseMovedEvent&&) noexcept = default;
  ~MouseMovedEvent() noexcept = default;

  auto operator=(const MouseMovedEvent&) -> MouseMovedEvent& = default;
  auto operator=(MouseMovedEvent&&) -> MouseMovedEvent& = default;

  math::Vec2i32 mPos;
};

constexpr MouseMovedEvent::MouseMovedEvent(const math::Vec2i32& pos) noexcept
: mPos(pos) {}


struct MouseButtonPressedEvent : EventTyped<EventType::MouseButtonPressed> {
  constexpr explicit MouseButtonPressedEvent(MouseButton button) noexcept;
  constexpr MouseButtonPressedEvent(const MouseButtonPressedEvent&) noexcept
    = default;
  constexpr MouseButtonPressedEvent(MouseButtonPressedEvent&&) noexcept
    = default;
  ~MouseButtonPressedEvent() noexcept = default;

  auto operator=(const MouseButtonPressedEvent&) -> MouseButtonPressedEvent&
    = default;
  auto operator=(MouseButtonPressedEvent&&) -> MouseButtonPressedEvent&
    = default;

  MouseButton mButton = MouseButton::Unknown;
};

constexpr MouseButtonPressedEvent::MouseButtonPressedEvent(
  const MouseButton button
) noexcept : mButton(button) {}


struct MouseButtonReleasedEvent : EventTyped<EventType::MouseButtonReleased> {
  constexpr explicit MouseButtonReleasedEvent(MouseButton button) noexcept;
  constexpr MouseButtonReleasedEvent(const MouseButtonReleasedEvent&) noexcept
    = default;
  constexpr MouseButtonReleasedEvent(MouseButtonReleasedEvent&&) noexcept
    = default;
  ~MouseButtonReleasedEvent() noexcept = default;

  auto operator=(const MouseButtonReleasedEvent&) -> MouseButtonReleasedEvent&
    = default;
  auto operator=(MouseButtonReleasedEvent&&) -> MouseButtonReleasedEvent&
    = default;

  MouseButton mButton = MouseButton::Unknown;
};

constexpr MouseButtonReleasedEvent::MouseButtonReleasedEvent(
  const MouseButton button
) noexcept : mButton(button) {}


struct MouseWheelScrolledEvent : EventTyped<EventType::MouseWheelScrolled> {
  constexpr explicit MouseWheelScrolledEvent(f32 offset) noexcept;
  constexpr MouseWheelScrolledEvent(const MouseWheelScrolledEvent&) noexcept = default;
  constexpr MouseWheelScrolledEvent(MouseWheelScrolledEvent&&) noexcept = default;
  ~MouseWheelScrolledEvent() noexcept = default;

  auto operator=(const MouseWheelScrolledEvent&) -> MouseWheelScrolledEvent&
    = default;
  auto operator=(MouseWheelScrolledEvent&&) -> MouseWheelScrolledEvent&
    = default;

  f32 mOffset = 0.0f;
};

constexpr MouseWheelScrolledEvent::MouseWheelScrolledEvent(
  const f32 offset
) noexcept
: mOffset(offset) {}

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
