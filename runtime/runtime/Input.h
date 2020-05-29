#pragma once
#ifndef BASALT_INPUT_H
#define BASALT_INPUT_H

#include "math/Vec2.h"
#include "shared/Asserts.h"
#include "shared/Types.h"

#include <bitset>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace basalt {

enum class MouseButton : u8 {
  Left, Right, Middle, Button4, Button5
};

constexpr uSize MOUSE_BUTTON_COUNT = 5u;

// TODO: add super/meta key for linux/osx
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

struct InputEvent;
using InputEventPtr = std::unique_ptr<InputEvent>;

struct Input final {
  Input() = default;

  Input(const Input&) = delete;
  Input(Input&&) = default;

  ~Input() = default;

  auto operator=(const Input&) -> Input& = delete;
  auto operator=(Input&&) -> Input& = default;

  [[nodiscard]]
  auto events() const -> const std::vector<InputEventPtr>&;

  [[nodiscard]]
  auto mouse_position() const -> math::Vec2i32;
  void mouse_moved(i32 x, i32 y);

  void mouse_wheel(f32 offset);

  [[nodiscard]]
  auto is_mouse_button_down(MouseButton) const -> bool;
  void mouse_button_pressed(MouseButton);
  void mouse_button_released(MouseButton);

  [[nodiscard]]
  auto is_key_down(Key) const -> bool;
  void key_down(Key);
  void key_up(Key);

  void characters_typed(std::string);

private:
  std::vector<InputEventPtr> mEvents {};
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown {};
  math::Vec2i32 mMousePosition {};
  std::bitset<KEY_COUNT> mKeysDown {};
};

enum class InputEventType : u8 {
  Unknown
, MouseMoved
, MouseWheel
, MouseButtonPressed
, MouseButtonReleased
, KeyDown
, KeyUp
, CharactersTyped
};

struct InputEvent {
  InputEventType type {InputEventType::Unknown};

  constexpr InputEvent() noexcept = default;

  constexpr InputEvent(const InputEvent&) noexcept = default;
  constexpr InputEvent(InputEvent&&) noexcept = default;

  ~InputEvent() noexcept = default;

  auto operator=(const InputEvent&) noexcept -> InputEvent& = default;
  auto operator=(InputEvent&&) noexcept -> InputEvent& = default;

  // TODO: this is terrible.
  // This returns a raw ptr from the InputEventPtr which MUST NOT be stored
  template <typename T>
  auto as() -> T* {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid input event cast");
    return static_cast<T*>(this);
  }
};

template <InputEventType Type>
struct InputEventT : InputEvent {
  static constexpr InputEventType TYPE = Type;

  constexpr InputEventT() noexcept
    : InputEvent {TYPE} {
  }

  constexpr InputEventT(const InputEventT&) noexcept = default;
  constexpr InputEventT(InputEventT&&) noexcept = default;

  ~InputEventT() noexcept = default;

  auto operator=(const InputEventT&) noexcept -> InputEventT& = default;
  auto operator=(InputEventT&&) noexcept -> InputEventT& = default;
};

struct MouseMoved final : InputEventT<InputEventType::MouseMoved> {
  math::Vec2i32 position;

  constexpr explicit MouseMoved(const math::Vec2i32& pos) noexcept
    : position {pos} {
  }

  constexpr MouseMoved(const MouseMoved&) noexcept = default;
  constexpr MouseMoved(MouseMoved&&) noexcept = default;

  ~MouseMoved() noexcept = default;

  auto operator=(const MouseMoved&) -> MouseMoved& = default;
  auto operator=(MouseMoved&&) -> MouseMoved& = default;
};

struct MouseWheel final : InputEventT<InputEventType::MouseWheel> {
  f32 offset;

  constexpr explicit MouseWheel(const f32 offset) noexcept
    : offset {offset} {
  }

  constexpr MouseWheel(const MouseWheel&) noexcept = default;
  constexpr MouseWheel(MouseWheel&&) noexcept = default;

  ~MouseWheel() noexcept = default;

  auto operator=(const MouseWheel&) -> MouseWheel& = default;
  auto operator=(MouseWheel&&) -> MouseWheel& = default;
};

struct MouseButtonPressed final
  : InputEventT<InputEventType::MouseButtonPressed> {
  MouseButton button;

  constexpr explicit MouseButtonPressed(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonPressed(const MouseButtonPressed&) noexcept = default;
  constexpr MouseButtonPressed(MouseButtonPressed&&) noexcept = default;

  ~MouseButtonPressed() noexcept = default;

  auto operator=(const MouseButtonPressed&) -> MouseButtonPressed& = default;
  auto operator=(MouseButtonPressed&&) -> MouseButtonPressed& = default;
};

struct MouseButtonReleased final
  : InputEventT<InputEventType::MouseButtonReleased> {
  MouseButton button;

  constexpr explicit MouseButtonReleased(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonReleased(const MouseButtonReleased&) noexcept = default;
  constexpr MouseButtonReleased(MouseButtonReleased&&) noexcept = default;

  ~MouseButtonReleased() noexcept = default;

  auto operator=(const MouseButtonReleased&) -> MouseButtonReleased& = default;
  auto operator=(MouseButtonReleased&&) -> MouseButtonReleased& = default;
};

struct KeyDown final : InputEventT<InputEventType::KeyDown> {
  Key key;

  constexpr explicit KeyDown(const Key key) noexcept
    : key {key} {
  }

  constexpr KeyDown(const KeyDown&) noexcept = default;
  constexpr KeyDown(KeyDown&&) noexcept = default;

  ~KeyDown() noexcept = default;

  auto operator=(const KeyDown&) noexcept -> KeyDown& = default;
  auto operator=(KeyDown&&) noexcept -> KeyDown& = default;
};

struct KeyUp : InputEventT<InputEventType::KeyUp> {
  Key key;

  constexpr explicit KeyUp(const Key key) noexcept
    : key(key) {
  }

  constexpr KeyUp(const KeyUp&) noexcept = default;
  constexpr KeyUp(KeyUp&&) noexcept = default;

  ~KeyUp() noexcept = default;

  auto operator=(const KeyUp&) noexcept -> KeyUp& = default;
  auto operator=(KeyUp&&) noexcept -> KeyUp& = default;
};

struct CharactersTyped final : InputEventT<InputEventType::CharactersTyped> {
  // TODO: optimization: use static char array instead of string?
  std::string chars;

  explicit CharactersTyped(std::string chars)
    : chars(std::move(chars)) {
  }

  CharactersTyped(const CharactersTyped&) = default;
  CharactersTyped(CharactersTyped&&) noexcept = default;

  ~CharactersTyped() noexcept = default;

  auto operator=(const CharactersTyped&) -> CharactersTyped& = default;
  auto operator=(CharactersTyped&&) -> CharactersTyped& = default;
};

} // namespace basalt

#endif // !BASALT_INPUT_H
