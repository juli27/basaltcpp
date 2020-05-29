#pragma once
#ifndef BASALT_PLATFORM_EVENTS_KEY_EVENTS_H
#define BASALT_PLATFORM_EVENTS_KEY_EVENTS_H

#include "Event.h"

#include "runtime/shared/Types.h"

namespace basalt::platform {

// TODO: readd super/meta key for linux/osx
//       should not map to windows key on windows
enum class Key : u8 {
  Unknown = 0,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  Escape, Tab, CapsLock, Shift, Control, Alt, /*Super,*/
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


struct KeyPressedEvent final : EventTyped<EventType::KeyPressed> {
  Key key {Key::Unknown};

  constexpr explicit KeyPressedEvent(const Key key) noexcept
    : key {key} {
  }

  constexpr KeyPressedEvent(const KeyPressedEvent&) noexcept = default;
  constexpr KeyPressedEvent(KeyPressedEvent&&) noexcept = default;

  ~KeyPressedEvent() noexcept = default;

  auto operator=(const KeyPressedEvent&) noexcept -> KeyPressedEvent& = default;
  auto operator=(KeyPressedEvent&&) noexcept -> KeyPressedEvent& = default;
};

struct KeyReleasedEvent : EventTyped<EventType::KeyReleased> {
  Key key {Key::Unknown};

  constexpr explicit KeyReleasedEvent(const Key key) noexcept
    : key(key) {
  }

  constexpr KeyReleasedEvent(const KeyReleasedEvent&) noexcept = default;
  constexpr KeyReleasedEvent(KeyReleasedEvent&&) noexcept = default;

  ~KeyReleasedEvent() noexcept = default;

  auto operator=(
    const KeyReleasedEvent&) noexcept -> KeyReleasedEvent& = default;
  auto operator=(KeyReleasedEvent&&) noexcept -> KeyReleasedEvent& = default;
};

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_EVENTS_KEY_EVENTS_H
