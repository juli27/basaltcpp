#pragma once
#ifndef BS_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BS_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include <basalt/common/Types.h>
#include <basalt/math/Vec2.h>

namespace basalt {
namespace platform {


enum class MouseButton : i8 {
  UNKNOWN = 0,
  LEFT, RIGHT, MIDDLE, BUTTON4, BUTTON5,

  NUMBER_OF_BUTTONS
};


struct MouseMovedEvent : EventTyped<EventType::MOUSE_MOVED> {
  math::Vec2i32 pos;
};


struct MouseButtonPressedEvent : EventTyped<EventType::MOUSE_BUTTON_PRESSED> {
  MouseButton button = MouseButton::UNKNOWN;
};


struct MouseButtonReleasedEvent : EventTyped<EventType::MOUSE_BUTTON_RELEASED> {
  MouseButton button = MouseButton::UNKNOWN;
};

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_MOUSE_EVENTS_H
