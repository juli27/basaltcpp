#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/base/types.h>
#include <basalt/api/base/vec.h>

#include <memory>

namespace basalt {

struct Engine;

struct InputEvent;
using InputEventPtr = std::unique_ptr<InputEvent>;

struct SceneView;
using SceneViewPtr = std::shared_ptr<SceneView>;

struct View;
using ViewPtr = std::shared_ptr<View>;

enum class InputEventHandled : u8 { No, Yes };

enum class InputEventType : u8 {
  MouseMoved,
  MouseWheel,
  MouseButtonDown,
  MouseButtonUp,
  KeyDown,
  KeyUp,
  CharacterTyped
};

enum class MouseButton : u8 { Left, Right, Middle, Button4, Button5 };
constexpr uSize MOUSE_BUTTON_COUNT = 5u;

enum class MouseCursor : u8 {
  Arrow,
  TextInput,
  ResizeAll,
  ResizeNS,
  ResizeEW,
  ResizeNESW,
  ResizeNWSE,
  Hand,
  NotAllowed
};
constexpr uSize MOUSE_CURSOR_COUNT = 9u;

struct PointerPosition : vec<PointerPosition, i32, 2> {
  [[nodiscard]] constexpr auto x() const noexcept -> i32 {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto y() const noexcept -> i32 {
    return std::get<1>(elements);
  }
};

// TODO: add super/meta key for linux/osx
//       should not map to windows key on windows
enum class Key : u8 {
  // clang-format off
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
  // clang-format on
};

constexpr uSize KEY_COUNT = 102u;

} // namespace basalt
