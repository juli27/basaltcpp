#pragma once
#ifndef BASALT_PLATFORM_EVENTS_EVENT_H
#define BASALT_PLATFORM_EVENTS_EVENT_H

#include "runtime/shared/Types.h"

namespace basalt::platform {

enum class EventType : u8 {
  Unknown = 0
, KeyPressed
, KeyReleased
, CharactersTyped
};


struct Event {
  EventType type {EventType::Unknown};

  constexpr Event() noexcept = default;

  constexpr Event(const Event&) noexcept = default;
  constexpr Event(Event&&) noexcept = default;

  ~Event() noexcept = default;

  auto operator=(const Event&) noexcept -> Event& = default;
  auto operator=(Event&&) noexcept -> Event& = default;
};

template <EventType Type>
struct EventTyped : Event {
  static constexpr EventType TYPE = Type;

  constexpr EventTyped() noexcept
    : Event {TYPE} {
  }

  constexpr EventTyped(const EventTyped&) noexcept = default;
  constexpr EventTyped(EventTyped&&) noexcept = default;

  ~EventTyped() noexcept = default;

  auto operator=(const EventTyped&) noexcept -> EventTyped& = default;
  auto operator=(EventTyped&&) noexcept -> EventTyped& = default;
};

struct EventDispatcher final {
  explicit EventDispatcher(const Event& event)
    : mEvent(event) {
  }

  template <typename T, typename EventFn>
  void dispatch(EventFn func) const {
    if (mEvent.type == T::TYPE) {
      func(*static_cast<const T*>(&mEvent));
    }
  }

private:
  const Event& mEvent;
};

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_EVENT_H
