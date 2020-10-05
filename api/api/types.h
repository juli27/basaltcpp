#pragma once

#include "shared/types.h"

namespace basalt {

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

} // namespace basalt
