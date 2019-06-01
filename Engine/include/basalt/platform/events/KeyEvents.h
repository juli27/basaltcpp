#pragma once
#ifndef BS_PLATFORM_EVENTS_KEY_EVENTS_H
#define BS_PLATFORM_EVENTS_KEY_EVENTS_H

#include "Event.h"

#include <basalt/common/Types.h>

namespace basalt {
namespace platform {

class KeyEvent : public Event {
public:
  constexpr Key GetKey() const;

protected:
  constexpr KeyEvent(Key key);

private:
  const Key m_key;
};


constexpr Key KeyEvent::GetKey() const {
  return m_key;
}


constexpr KeyEvent::KeyEvent(Key key) : m_key(key) {}


class KeyPressedEvent final : public KeyEvent {
public:
  constexpr KeyPressedEvent(Key key, i32 repeatCount);

  virtual inline EventType GetEventType() const override;
  constexpr i32 GetRepeatCount() const;

private:
  const i32 m_repeatCount;

public:
  static constexpr EventType EVENT_TYPE = EventType::KEY_PRESSED;
};


constexpr KeyPressedEvent::KeyPressedEvent(Key key, i32 repeatCount)
  : KeyEvent(key)
  , m_repeatCount(repeatCount) {}


inline EventType KeyPressedEvent::GetEventType() const {
  return EVENT_TYPE;
}


constexpr i32 KeyPressedEvent::GetRepeatCount() const {
  return m_repeatCount;
}


class KeyReleasedEvent final : public KeyEvent {
public:
  constexpr KeyReleasedEvent(Key key);

  virtual inline EventType GetEventType() const override;

public:
  static constexpr EventType EVENT_TYPE = EventType::KEY_RELEASED;
};


constexpr KeyReleasedEvent::KeyReleasedEvent(Key key) : KeyEvent(key) {}


inline EventType KeyReleasedEvent::GetEventType() const {
  return EVENT_TYPE;
}

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_KEY_EVENTS_H
