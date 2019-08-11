#pragma once
#ifndef BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
#define BS_PLATFORM_EVENTS_WINDOW_EVENTS_H

#include "Event.h"

#include <basalt/math/Vec2.h>

namespace basalt::platform {


struct WindowResizedEvent : EventTyped<EventType::WINDOW_RESIZED> {
  math::Vec2i32 newSize;
};

} // namespace basalt::platform

#endif // !BS_PLATFORM_EVENTS_WINDOW_EVENTS_H
