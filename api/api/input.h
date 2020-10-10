#pragma once

#include "shared/asserts.h"

#include "base/types.h"
#include "base/vec.h"

#include <bitset>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace basalt {

enum class MouseButton : u8 { Left, Right, Middle, Button4, Button5 };
constexpr uSize MOUSE_BUTTON_COUNT = 5u;

// TODO: add super/meta key for linux/osx
//       should not map to windows key on windows
// clang-format off
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

// clang-format on

constexpr uSize KEY_COUNT = 102u;

struct InputEvent;
using InputEventPtr = std::unique_ptr<InputEvent>;

struct CursorPosition : vec<CursorPosition, i32, 2> {
  [[nodiscard]] constexpr auto x() const noexcept -> i32 {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto y() const noexcept -> i32 {
    return std::get<1>(elements);
  }
};

struct Input final {
  Input() = default;

  Input(const Input&) = delete;
  Input(Input&&) = default;

  ~Input() = default;

  auto operator=(const Input&) -> Input& = delete;
  auto operator=(Input &&) -> Input& = delete;

  [[nodiscard]] auto events() const noexcept
    -> const std::vector<InputEventPtr>&;

  [[nodiscard]] auto cursor_position() const noexcept -> CursorPosition;
  void mouse_moved(CursorPosition);

  void mouse_wheel(f32 offset);

  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  void mouse_button_down(MouseButton);
  void mouse_button_up(MouseButton);

  [[nodiscard]] auto is_key_down(Key) const -> bool;
  void key_down(Key);
  void key_up(Key);

  void characters_typed(std::string);

private:
  std::vector<InputEventPtr> mEvents {};
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown {};
  CursorPosition mMousePosition {};
  std::bitset<KEY_COUNT> mKeysDown {};
};

enum class InputEventType : u8 {
  MouseMoved,
  MouseWheel,
  MouseButtonDown,
  MouseButtonUp,
  KeyDown,
  KeyUp,
  CharactersTyped
};

struct InputEvent {
  const InputEventType type;

  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid input event cast");
    return *static_cast<T*>(this);
  }

protected:
  constexpr explicit InputEvent(const InputEventType eType) noexcept
    : type {eType} {
  }
};

template <InputEventType Type>
struct InputEventT : InputEvent {
  static constexpr InputEventType TYPE = Type;

protected:
  constexpr InputEventT() noexcept : InputEvent {TYPE} {
  }
};

struct MouseMoved final : InputEventT<InputEventType::MouseMoved> {
  CursorPosition position;

  constexpr explicit MouseMoved(const CursorPosition& pos) noexcept
    : position {pos} {
  }
};

struct MouseWheel final : InputEventT<InputEventType::MouseWheel> {
  f32 offset;

  constexpr explicit MouseWheel(const f32 offset) noexcept : offset {offset} {
  }
};

struct MouseButtonDown final : InputEventT<InputEventType::MouseButtonDown> {
  MouseButton button;

  constexpr explicit MouseButtonDown(const MouseButton button) noexcept
    : button {button} {
  }
};

struct MouseButtonUp final : InputEventT<InputEventType::MouseButtonUp> {
  MouseButton button;

  constexpr explicit MouseButtonUp(const MouseButton button) noexcept
    : button {button} {
  }
};

struct KeyDown final : InputEventT<InputEventType::KeyDown> {
  Key key;

  constexpr explicit KeyDown(const Key key) noexcept : key {key} {
  }
};

struct KeyUp : InputEventT<InputEventType::KeyUp> {
  Key key;

  constexpr explicit KeyUp(const Key key) noexcept : key(key) {
  }
};

struct CharactersTyped final : InputEventT<InputEventType::CharactersTyped> {
  std::string chars;

  explicit CharactersTyped(std::string chars) : chars(std::move(chars)) {
  }
};

} // namespace basalt
