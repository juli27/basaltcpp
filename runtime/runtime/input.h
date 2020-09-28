#pragma once

#include "math/vec2.h"
#include "shared/asserts.h"
#include "shared/types.h"

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

struct CursorPosition : Vec2i32 {
  using Vec2i32::Vec2i32;
};

struct Input final {
  Input() = default;

  Input(const Input&) = delete;
  Input(Input&&) = default;

  ~Input() = default;

  auto operator=(const Input&) -> Input& = delete;
  auto operator=(Input&&) -> Input& = delete;

  [[nodiscard]]
  auto events() const -> const std::vector<InputEventPtr>&;

  [[nodiscard]]
  auto cursor_position() const -> CursorPosition;
  void mouse_moved(CursorPosition);

  void mouse_wheel(f32 offset);

  [[nodiscard]]
  auto is_mouse_button_down(MouseButton) const -> bool;
  void mouse_button_down(MouseButton);
  void mouse_button_up(MouseButton);

  [[nodiscard]]
  auto is_key_down(Key) const -> bool;
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
  MouseMoved
, MouseWheel
, MouseButtonDown
, MouseButtonUp
, KeyDown
, KeyUp
, CharactersTyped
};

struct InputEvent {
  InputEventType type;

  constexpr InputEvent() noexcept = delete;

  constexpr InputEvent(const InputEvent&) noexcept = default;
  constexpr InputEvent(InputEvent&&) noexcept = default;

  ~InputEvent() noexcept = default;

  auto operator=(const InputEvent&) noexcept -> InputEvent& = default;
  auto operator=(InputEvent&&) noexcept -> InputEvent& = default;

  // TODO: is there a better solution?
  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid input event cast");
    return *static_cast<T*>(this);
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
  CursorPosition position;

  constexpr explicit MouseMoved(const CursorPosition& pos) noexcept
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

struct MouseButtonDown final : InputEventT<InputEventType::MouseButtonDown> {
  MouseButton button;

  constexpr explicit MouseButtonDown(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonDown(const MouseButtonDown&) noexcept = default;
  constexpr MouseButtonDown(MouseButtonDown&&) noexcept = default;

  ~MouseButtonDown() noexcept = default;

  auto operator=(const MouseButtonDown&) -> MouseButtonDown& = default;
  auto operator=(MouseButtonDown&&) -> MouseButtonDown& = default;
};

struct MouseButtonUp final : InputEventT<InputEventType::MouseButtonUp> {
  MouseButton button;

  constexpr explicit MouseButtonUp(const MouseButton button) noexcept
    : button {button} {
  }

  constexpr MouseButtonUp(const MouseButtonUp&) noexcept = default;
  constexpr MouseButtonUp(MouseButtonUp&&) noexcept = default;

  ~MouseButtonUp() noexcept = default;

  auto operator=(const MouseButtonUp&) -> MouseButtonUp& = default;
  auto operator=(MouseButtonUp&&) -> MouseButtonUp& = default;
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
