#pragma once
#ifndef BS_PLATFORM_EVENTS_MOUSE_EVENTS_H
#define BS_PLATFORM_EVENTS_MOUSE_EVENTS_H

#include "Event.h"

#include <basalt/common/Types.h>
#include <basalt/math/Vec2.h>

namespace basalt {
namespace platform {


enum class MouseButton : i32 {
  UNKNOWN = 0,
  LEFT, RIGHT, MIDDLE, BUTTON4, BUTTON5,

  NUMBER_OF_BUTTONS
};


class MouseMovedEvent final : public Event {
public:
  constexpr MouseMovedEvent(math::Vec2i32 pos);

  virtual inline EventType GetEventType() const override;
  constexpr const math::Vec2i32& GetMousePos() const;

private:
  const math::Vec2i32 m_pos;

public:
  static constexpr EventType EVENT_TYPE = EventType::MOUSE_MOVED;
};


constexpr MouseMovedEvent::MouseMovedEvent(math::Vec2i32 pos) : m_pos(pos) {}


inline EventType MouseMovedEvent::GetEventType() const {
  return EVENT_TYPE;
}


constexpr const math::Vec2i32& MouseMovedEvent::GetMousePos() const {
  return m_pos;
}


class MouseButtonEvent : public Event {
public:
  constexpr MouseButton GetButton() const;

protected:
  constexpr MouseButtonEvent(MouseButton button);

private:
  const MouseButton m_button;
};


constexpr MouseButton MouseButtonEvent::GetButton() const {
  return m_button;
}


constexpr MouseButtonEvent::MouseButtonEvent(MouseButton button)
  : m_button(button) {}


class MouseButtonPressedEvent final : public MouseButtonEvent {
public:
  constexpr MouseButtonPressedEvent(MouseButton button);

  virtual inline EventType GetEventType() const override;

public:
  static constexpr EventType EVENT_TYPE = EventType::MOUSE_BUTTON_PRESSED;
};


constexpr MouseButtonPressedEvent::MouseButtonPressedEvent(MouseButton button)
  : MouseButtonEvent(button) {}


inline EventType MouseButtonPressedEvent::GetEventType() const {
  return EVENT_TYPE;
}


class MouseButtonReleasedEvent final : public MouseButtonEvent {
public:
  constexpr MouseButtonReleasedEvent(MouseButton button);

  virtual inline EventType GetEventType() const override;

public:
  static constexpr EventType EVENT_TYPE = EventType::MOUSE_BUTTON_RELEASED;
};


constexpr MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseButton button)
  : MouseButtonEvent(button) {}


inline EventType MouseButtonReleasedEvent::GetEventType() const {
  return EVENT_TYPE;
}

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_MOUSE_EVENTS_H
