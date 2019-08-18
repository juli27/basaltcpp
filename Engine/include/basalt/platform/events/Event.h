#pragma once
#ifndef BS_PLATFORM_EVENTS_EVENT_H
#define BS_PLATFORM_EVENTS_EVENT_H

#include <basalt/common/Types.h>

namespace basalt::platform {


enum class EventType : i8 {
  Unknown = 0,
  WindowResized,
  KeyPressed,
  KeyReleased,
  MouseMoved,
  MouseButtonPressed,
  MouseButtonReleased,
  CharactersTyped
};


struct Event {
  constexpr explicit Event(EventType type) noexcept;
  constexpr Event(const Event&) noexcept = default;
  constexpr Event(Event&&) noexcept = default;
  inline ~Event() noexcept = default;

  auto operator=(const Event&) noexcept -> Event& = default;
  auto operator=(Event&&) noexcept -> Event& = default;

  EventType mType = EventType::Unknown;
};


constexpr Event::Event(const EventType type) noexcept : mType(type) {}


template <EventType Type>
struct EventTyped : Event {
  static constexpr EventType TYPE = Type;

  constexpr EventTyped() noexcept;
  constexpr EventTyped(const EventTyped&) noexcept = default;
  constexpr EventTyped(EventTyped&&) noexcept = default;
  inline ~EventTyped() noexcept = default;

  auto operator=(const EventTyped&) noexcept -> EventTyped& = default;
  auto operator=(EventTyped&&) noexcept -> EventTyped& = default;
};


template <EventType Type>
constexpr EventTyped<Type>::EventTyped() noexcept: Event(TYPE) {}


class EventDispatcher {
public:
  inline explicit EventDispatcher(const Event& event);

  template <typename T, typename EventFn>
  inline void Dispatch(EventFn func) const;

private:
  const Event& mEvent;
};


inline EventDispatcher::EventDispatcher(const Event& event) : mEvent(event) {}


template<typename T, typename EventFn>
inline void EventDispatcher::Dispatch(EventFn func) const {
  if (mEvent.mType == T::TYPE) {
    func(*static_cast<const T*>(&mEvent));
  }
}

} // namespace basalt::platform

#endif // !BS_PLATFORM_EVENTS_EVENT_H
