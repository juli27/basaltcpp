#pragma once
#ifndef BASALT_PLATFORM_EVENTS_KEY_EVENTS_H
#define BASALT_PLATFORM_EVENTS_KEY_EVENTS_H

#include "Event.h"

#include "runtime/shared/Types.h"

#include <string>
#include <utility> // move

namespace basalt::platform {

enum class Key : u8 {
  Unknown = 0,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  Escape, Tab, CapsLock, Shift, Control, Alt, Super,
  Insert, Delete, Home, End, PageUp, PageDown, Pause,
  LeftArrow, RightArrow, UpArrow, DownArrow,
  Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
  Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
  NumpadAdd, NumpadSub, NumpadMul, NumpadDiv,
  NumpadDecimal, NumpadLock, NumpadEnter,
  Zero, One, Two, Three, Four,
  Five, Six, Seven, Eight, Nine,
  Backspace, Space, Enter, Menu,
  ScrollLock, Print, Plus,
  A, B, C, D, E, F, G, H, I, J, K, L, M,
  N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  Minus,
  Comma, Period,
  Oem1, Oem2, Oem3, Oem4, Oem5, Oem6, Oem7, Oem8, Oem9,
};

constexpr uSize KEY_COUNT = 103u;


struct KeyPressedEvent : EventTyped<EventType::KeyPressed> {
  constexpr explicit KeyPressedEvent(Key key) noexcept;
  constexpr KeyPressedEvent(const KeyPressedEvent&) noexcept = default;
  constexpr KeyPressedEvent(KeyPressedEvent&&) noexcept = default;
  ~KeyPressedEvent() noexcept = default;

  auto operator=(const KeyPressedEvent&) noexcept -> KeyPressedEvent& = default;
  auto operator=(KeyPressedEvent&&) noexcept -> KeyPressedEvent& = default;

  Key mKey = Key::Unknown;
};

constexpr KeyPressedEvent::KeyPressedEvent(const Key key) noexcept
: mKey(key) {}


struct KeyReleasedEvent : EventTyped<EventType::KeyReleased> {
  constexpr explicit KeyReleasedEvent(Key key) noexcept;
  constexpr KeyReleasedEvent(const KeyReleasedEvent&) noexcept = default;
  constexpr KeyReleasedEvent(KeyReleasedEvent&&) noexcept = default;
  ~KeyReleasedEvent() noexcept = default;

  auto operator=(const KeyReleasedEvent&) noexcept -> KeyReleasedEvent&
    = default;
  auto operator=(KeyReleasedEvent&&) noexcept -> KeyReleasedEvent& = default;

  Key mKey = Key::Unknown;
};

constexpr KeyReleasedEvent::KeyReleasedEvent(const Key key) noexcept
: mKey(key) {}


struct CharactersTyped : EventTyped<EventType::CharactersTyped> {
  inline explicit CharactersTyped(std::string chars);
  CharactersTyped(const CharactersTyped&) = default;
  CharactersTyped(CharactersTyped&&) noexcept = default;
  ~CharactersTyped() noexcept = default;

  auto operator=(const CharactersTyped&) -> CharactersTyped& = default;
  auto operator=(CharactersTyped&&) -> CharactersTyped& = default;

  // TODO: optimization: use static char array instead of string?
  std::string mChars;
};

inline CharactersTyped::CharactersTyped(std::string chars)
: mChars(std::move(chars)) {}

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_KEY_EVENTS_H