#pragma once
#ifndef BS_PLATFORM_EVENTS_KEY_EVENTS_H
#define BS_PLATFORM_EVENTS_KEY_EVENTS_H

#include <string>

#include "Event.h"

#include <basalt/common/Types.h>

namespace basalt {
namespace platform {


enum class Key : i32 {
  UNKNOWN = 0,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  ESCAPE, TAB, CAPS_LOCK, SHIFT, CONTROL, ALT, SUPER,
  INSERT, DELETE, HOME, END, PAGE_UP, PAGE_DOWN, PAUSE,
  LEFT_ARROW, RIGHT_ARROW, UP_ARROW, DOWN_ARROW,
  NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
  NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
  NUM_ADD, NUM_SUB, NUM_MUL, NUM_DIV,
  NUM_DECIMAL, NUM_LOCK, NUM_ENTER,
  ROW_0, ROW_1, ROW_2, ROW_3, ROW_4,
  ROW_5, ROW_6, ROW_7, ROW_8, ROW_9,
  BACKSPACE, SPACE, ENTER, MENU,
  SCROLL_LOCK, PRINT, PLUS,
  A, B, C, D, E, F, G, H, I, J, K, L, M,
  N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  MINUS,
  COMMA, PERIOD,
  OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8, OEM_9,

  NUMBER_OF_KEYS
};


struct KeyPressedEvent : EventTyped<EventType::KEY_PRESSED> {
  Key key = Key::UNKNOWN;
};


struct KeyReleasedEvent : EventTyped<EventType::KEY_RELEASED> {
  Key key = Key::UNKNOWN;
};


struct CharactersTyped : EventTyped<EventType::CHARACTERS_TYPED> {
  std::string chars;
};

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_KEY_EVENTS_H
