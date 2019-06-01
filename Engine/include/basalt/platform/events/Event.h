#pragma once
#ifndef BS_PLATFORM_EVENTS_EVENT_H
#define BS_PLATFORM_EVENTS_EVENT_H

#include <functional>

#include <basalt/common/Types.h>
#include <basalt/platform/WindowTypes.h>

#define BIND_EVENT_FN(f) std::bind(f, this, std::placeholders::_1)

namespace basalt {
namespace platform {

enum class EventType : i8 {
  UNKNOWN = 0,
  WINDOW_RESIZED,
  //WINDOW_CLOSE_REQUEST,
  KEY_PRESSED,
  KEY_RELEASED,
  MOUSE_MOVED,
  MOUSE_BUTTON_PRESSED,
  MOUSE_BUTTON_RELEASED,
};


class Event {
public:
  virtual ~Event() = default;

  virtual inline EventType GetEventType() const = 0;
};


class EventDispatcher final {
  template <typename T>
  using EventFn = std::function<void (const T&)>;

public:
  inline EventDispatcher(const Event& event);

  template <typename T>
  inline void Dispatch(EventFn<T> func) const;

private:
  const Event& m_event;
};


inline EventDispatcher::EventDispatcher(const Event& event) : m_event(event) {}


template<typename T>
inline void EventDispatcher::Dispatch(EventFn<T> func) const {
  if (m_event.GetEventType() == T::EVENT_TYPE) {
    func(*reinterpret_cast<const T*>(&m_event));
  }
}

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_EVENT_H
