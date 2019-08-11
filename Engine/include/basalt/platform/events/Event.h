#pragma once
#ifndef BS_PLATFORM_EVENTS_EVENT_H
#define BS_PLATFORM_EVENTS_EVENT_H

#include <basalt/common/Types.h>

namespace basalt::platform {


enum class EventType : i8 {
  UNKNOWN = 0,
  WINDOW_RESIZED,
  KEY_PRESSED,
  KEY_RELEASED,
  MOUSE_MOVED,
  MOUSE_BUTTON_PRESSED,
  MOUSE_BUTTON_RELEASED,
  CHARACTERS_TYPED
};


struct Event {
  EventType type = EventType::UNKNOWN;
};


template <EventType TYPE>
struct EventTyped : Event {
  static const EventType s_Type = TYPE;

  EventTyped() { type = s_Type; }
};


class EventDispatcher {
public:
  inline EventDispatcher(const Event& event);

  template <typename T, typename EventFn>
  inline void Dispatch(EventFn func) const;

private:
  const Event& m_event;
};


inline EventDispatcher::EventDispatcher(const Event& event) : m_event(event) {}


template<typename T, typename EventFn>
inline void EventDispatcher::Dispatch(EventFn func) const {
  if (m_event.type == T::s_Type) {
    func(*static_cast<const T*>(&m_event));
  }
}

} // namespace basalt::platform

#endif // !BS_PLATFORM_EVENTS_EVENT_H
