#pragma once

#include "types.h"

#include "shared/asserts.h"

#include "base/types.h"

#include <bitset>
#include <string>
#include <utility>
#include <vector>

namespace basalt {

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
  std::vector<InputEventPtr> mEvents;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  CursorPosition mMousePosition;
  std::bitset<KEY_COUNT> mKeysDown;
};

struct MouseMoved final : InputEventT<InputEventType::MouseMoved> {
  CursorPosition position;

  constexpr explicit MouseMoved(const CursorPosition& pos) noexcept
    : position {pos} {
  }
};

struct MouseWheel final : InputEventT<InputEventType::MouseWheel> {
  f32 offset;

  constexpr explicit MouseWheel(const f32 mouseWheelOffset) noexcept
    : offset {mouseWheelOffset} {
  }
};

struct MouseButtonDown final : InputEventT<InputEventType::MouseButtonDown> {
  MouseButton button;

  constexpr explicit MouseButtonDown(const MouseButton b) noexcept
    : button {b} {
  }
};

struct MouseButtonUp final : InputEventT<InputEventType::MouseButtonUp> {
  MouseButton button;

  constexpr explicit MouseButtonUp(const MouseButton b) noexcept : button {b} {
  }
};

struct KeyDown final : InputEventT<InputEventType::KeyDown> {
  Key key;

  constexpr explicit KeyDown(const Key k) noexcept : key {k} {
  }
};

struct KeyUp : InputEventT<InputEventType::KeyUp> {
  Key key;

  constexpr explicit KeyUp(const Key k) noexcept : key {k} {
  }
};

struct CharactersTyped final : InputEventT<InputEventType::CharactersTyped> {
  std::string chars;

  explicit CharactersTyped(std::string typedCharacters)
    : chars {std::move(typedCharacters)} {
  }
};

} // namespace basalt
