#include <basalt/api/canvas.h>

#include "canvas_p.h"

namespace basalt {

auto Canvas::mode() const -> CanvasMode {
  return mMode;
}

auto Canvas::next_mode() const -> std::optional<CanvasMode> {
  return mNextMode;
}

auto Canvas::set_next_mode(CanvasMode const nextMode) -> void {
  mNextMode = nextMode;
}

auto Canvas::cancel_next_mode() -> void {
  mNextMode.reset();
}

auto Canvas::pointer() const -> CanvasPointer {
  return mPointer;
}

auto Canvas::set_pointer(CanvasPointer const pointer) -> void {
  mPointer = pointer;
}

auto CanvasPrivate::set_mode(Canvas& canvas, CanvasMode const mode) -> void {
  canvas.mNextMode.reset();
  canvas.mMode = mode;
}

} // namespace basalt
