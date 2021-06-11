#pragma once

#include "types.h"

#include "shared/asserts.h"

#include "base/types.h"

#include <array>

namespace basalt {

struct InputEvent {
  const InputEventType type;

  template <typename T>
  [[nodiscard]] auto as() const -> const T& {
    BASALT_ASSERT(type == T::TYPE, "invalid input event cast");

    return *static_cast<const T*>(this);
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

// TODO: turn this into an relative mouse motion event.
//       The pointer position must be polled
struct MouseMoved final : InputEventT<InputEventType::MouseMoved> {
  PointerPosition position;

  constexpr explicit MouseMoved(const PointerPosition& pos) noexcept
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

static_assert(sizeof(KeyDown) == 2);

struct KeyUp : InputEventT<InputEventType::KeyUp> {
  Key key;

  constexpr explicit KeyUp(const Key k) noexcept : key {k} {
  }
};

struct CharacterTyped final : InputEventT<InputEventType::CharacterTyped> {
  std::array<char, 4> character {};

  explicit CharacterTyped(std::array<char, 4> character)
    : character {character} {
  }
};

} // namespace basalt
