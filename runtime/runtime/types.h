#pragma once
#ifndef BASALT_RUNTIME_TYPES_H
#define BASALT_RUNTIME_TYPES_H

#include "shared/Types.h"

namespace basalt {

enum class MouseCursor : u8 {
  Arrow, TextInput, ResizeAll, ResizeNS, ResizeEW, ResizeNESW, ResizeNWSE, Hand
, NotAllowed
};

constexpr uSize MOUSE_CURSOR_COUNT = 9u;

} // namespace basalt

#endif // BASALT_RUNTIME_TYPES_H
