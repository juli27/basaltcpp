#pragma once

#include "types.h"

#include "base/types.h"

#include <optional>

namespace basalt {

// SERIALIZED
enum class CanvasMode : u8 {
  Windowed = 0,
  Fullscreen = 1,
  FullscreenExclusive = 2,
};
inline auto constexpr CANVAS_MODE_COUNT = u8{3};

enum class CanvasPointer : u8 {
  Arrow,
  TextInput,
  ResizeAll,
  ResizeNS,
  ResizeEW,
  ResizeNESW,
  ResizeNWSE,
  Hand,
  NotAllowed,
  Wait,
  Progress,
};
inline auto constexpr CANVAS_POINTER_COUNT = u8{11};

class Canvas {
public:
  Canvas() = default;
  Canvas(Canvas const&) = delete;
  Canvas(Canvas&&) = delete;

  ~Canvas() = default;

  auto operator=(Canvas const&) -> Canvas& = delete;
  auto operator=(Canvas&&) -> Canvas& = delete;

  auto mode() const -> CanvasMode;
  auto next_mode() const -> std::optional<CanvasMode>;
  auto set_next_mode(CanvasMode) -> void;
  auto cancel_next_mode() -> void;

  auto pointer() const -> CanvasPointer;
  auto set_pointer(CanvasPointer) -> void;

private:
  CanvasMode mMode{CanvasMode::Windowed};
  std::optional<CanvasMode> mNextMode;
  CanvasPointer mPointer{CanvasPointer::Arrow};

  friend CanvasPrivate;
};

} // namespace basalt
