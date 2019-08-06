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
  constexpr KeyPressedEvent(Key key);

  virtual inline EventType GetEventType() const override;

public:
  static constexpr EventType EVENT_TYPE = EventType::KEY_PRESSED;
};


constexpr KeyPressedEvent::KeyPressedEvent(Key key)
  : KeyEvent(key) {}


inline EventType KeyPressedEvent::GetEventType() const {
  return EVENT_TYPE;
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


class CharactersTyped final : public Event {
public:
  constexpr CharactersTyped(const std::string& chars);

  virtual inline EventType GetEventType() const override;
  inline std::string_view GetTypedChars() const;

private:
  const std::string m_chars;

public:
  static constexpr EventType EVENT_TYPE = EventType::CHARACTERS_TYPED;
};


constexpr CharactersTyped::CharactersTyped(const std::string& chars)
  : m_chars(chars) {}


inline EventType CharactersTyped::GetEventType() const {
  return EVENT_TYPE;
}


inline std::string_view CharactersTyped::GetTypedChars() const {
  return m_chars;
}

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_EVENTS_KEY_EVENTS_H
