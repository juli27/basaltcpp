#pragma once
#ifndef BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
#define BS_PLATFORM_EVENTS_WINDOW_EVENTS_H

#include "Event.h"

#include <basalt/math/Vec2.h>

namespace basalt {
namespace platform {

class WindowResizedEvent final : public Event {
public:
  constexpr WindowResizedEvent(const math::Vec2i32& newSize);

  virtual inline EventType GetEventType() const override;
  constexpr const math::Vec2i32& GetNewSize() const;

private:
  const math::Vec2i32 m_newSize;

public:
  static constexpr EventType EVENT_TYPE = EventType::WINDOW_RESIZED;
};


constexpr WindowResizedEvent::WindowResizedEvent(const math::Vec2i32& newSize)
  : m_newSize(newSize) {}


inline EventType WindowResizedEvent::GetEventType() const {
  return EVENT_TYPE;
}


constexpr const math::Vec2i32& WindowResizedEvent::GetNewSize() const {
  return m_newSize;
}


//class WindowCloseRequestEvent final : public PlatformEvent {
//public:
//  inline WindowCloseRequestEvent();
//
//  virtual PlatformEventType GetEventType() const override;
//
//public:
//  static inline PlatformEventType GetStaticType();
//};
//
//inline WindowCloseRequestEvent::WindowCloseRequestEvent() {}
//
//inline PlatformEventType WindowCloseRequestEvent::GetEventType() const {
//  return GetStaticType();
//}
//
//inline PlatformEventType WindowCloseRequestEvent::GetStaticType() {
//  return PlatformEventType::WINDOW_CLOSE_REQUEST;
//}

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
