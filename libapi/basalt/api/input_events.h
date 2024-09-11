#pragma once

#include "types.h"

#include "base/asserts.h"
#include "base/types.h"

#include <array>
#include <functional>

namespace basalt {

struct InputEvent {
  InputEventType const type;

  template <typename T>
  [[nodiscard]]
  auto as() const -> T const& {
    BASALT_ASSERT(type == T::TYPE, "invalid input event cast");

    return *static_cast<T const*>(this);
  }

protected:
  constexpr explicit InputEvent(InputEventType const eType) noexcept
    : type{eType} {
  }
};

template <InputEventType Type>
struct InputEventT : InputEvent {
  static constexpr auto TYPE = Type;

protected:
  constexpr InputEventT() noexcept : InputEvent{TYPE} {
  }
};

// TODO: turn this into a relative mouse motion event.
//       The pointer position must be polled
struct MouseMoved final : InputEventT<InputEventType::MouseMoved> {
  PointerPosition position;

  constexpr explicit MouseMoved(PointerPosition const& pos) noexcept
    : position{pos} {
  }
};

struct MouseWheel final : InputEventT<InputEventType::MouseWheel> {
  f32 offset;

  constexpr explicit MouseWheel(f32 const mouseWheelOffset) noexcept
    : offset{mouseWheelOffset} {
  }
};

struct MouseButtonDown final : InputEventT<InputEventType::MouseButtonDown> {
  MouseButton button;

  constexpr explicit MouseButtonDown(MouseButton const b) noexcept : button{b} {
  }
};

struct MouseButtonUp final : InputEventT<InputEventType::MouseButtonUp> {
  MouseButton button;

  constexpr explicit MouseButtonUp(MouseButton const b) noexcept : button{b} {
  }
};

struct KeyboardFocusGained final
  : InputEventT<InputEventType::KeyboardFocusGained> {
  constexpr KeyboardFocusGained() noexcept = default;
};

struct KeyboardFocusLost final
  : InputEventT<InputEventType::KeyboardFocusLost> {
  constexpr KeyboardFocusLost() noexcept = default;
};

struct KeyDown final : InputEventT<InputEventType::KeyDown> {
  Key key;

  constexpr explicit KeyDown(Key const k) noexcept : key{k} {
  }
};

static_assert(sizeof(KeyDown) == 2);

struct KeyUp : InputEventT<InputEventType::KeyUp> {
  Key key;

  constexpr explicit KeyUp(Key const k) noexcept : key{k} {
  }
};

struct CharacterTyped final : InputEventT<InputEventType::CharacterTyped> {
  std::array<char, 4> character{};

  explicit CharacterTyped(std::array<char, 4> character)
    : character{character} {
  }
};

template <typename... Ts>
struct InputEventHandler : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
InputEventHandler(Ts...) -> InputEventHandler<Ts...>;

auto inline constexpr ignoreOtherEvents = [](InputEvent const&) {};

template <typename Handler>
auto handle(InputEvent const& e, Handler&& handler) {
#define VISIT(EventStruct)                                                     \
  case EventStruct::TYPE:                                                      \
    return std::invoke(handler, e.as<EventStruct>());

  switch (e.type) {
    VISIT(MouseMoved);
    VISIT(MouseWheel);
    VISIT(MouseButtonDown);
    VISIT(MouseButtonUp);
    VISIT(KeyboardFocusGained);
    VISIT(KeyboardFocusLost);
    VISIT(KeyDown);
    VISIT(KeyUp);
    VISIT(CharacterTyped);
  }

  BASALT_CRASH("unknown input event type");

#undef VISIT
}

} // namespace basalt
