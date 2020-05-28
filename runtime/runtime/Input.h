#pragma once
#ifndef BASALT_INPUT_H
#define BASALT_INPUT_H

#include "runtime/platform/events/KeyEvents.h"

#include "math/Vec2.h"
#include "shared/Asserts.h"
#include "shared/Types.h"

#include <bitset>
#include <memory>
#include <vector>

namespace basalt {

enum class MouseButton : u8 {
  Left, Right, Middle, Button4, Button5
};

constexpr uSize MOUSE_BUTTON_COUNT = 5u;

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
  auto is_mouse_button_down(MouseButton) const -> bool;
  void mouse_button_pressed(MouseButton);
  void mouse_button_released(MouseButton);

private:
  std::vector<InputEventPtr> mEvents {};
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown {};
};

enum class InputEventType : u8 {
  Unknown = 0
, MouseButtonPressed
, MouseButtonReleased
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

namespace input {

using platform::Key;
using platform::KEY_COUNT;

void init();

auto is_key_pressed(Key key) -> bool;

auto mouse_pos() -> math::Vec2i32;

} // namespace input
} // namespace basalt

#endif // !BASALT_INPUT_H
