#pragma once
#ifndef BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BASALT_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include "runtime/math/Vec2.h"
#include "runtime/shared/Types.h"

namespace basalt::platform {

enum class MouseButton : u8 {
  Unknown = 0,
  Left, Right, Middle, Button4, Button5,
};

constexpr uSize MOUSE_BUTTON_COUNT = 6u;


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

struct MouseButtonPressedEvent final
  : EventTyped<EventType::MouseButtonPressed> {
  MouseButton button {MouseButton::Unknown};

  constexpr explicit MouseButtonPressedEvent(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonPressedEvent(const MouseButtonPressedEvent&) noexcept
  = default;
  constexpr MouseButtonPressedEvent(MouseButtonPressedEvent&&) noexcept
  = default;

  ~MouseButtonPressedEvent() noexcept = default;

  auto operator=(
    const MouseButtonPressedEvent&) -> MouseButtonPressedEvent& = default;
  auto operator=(
    MouseButtonPressedEvent&&) -> MouseButtonPressedEvent& = default;
};

struct MouseButtonReleasedEvent final
  : EventTyped<EventType::MouseButtonReleased> {
  MouseButton button {MouseButton::Unknown};

  constexpr explicit MouseButtonReleasedEvent(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonReleasedEvent(const MouseButtonReleasedEvent&) noexcept
  = default;
  constexpr MouseButtonReleasedEvent(MouseButtonReleasedEvent&&) noexcept
  = default;

  ~MouseButtonReleasedEvent() noexcept = default;

  auto operator=(
    const MouseButtonReleasedEvent&) -> MouseButtonReleasedEvent& = default;
  auto operator=(
    MouseButtonReleasedEvent&&) -> MouseButtonReleasedEvent& = default;
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
