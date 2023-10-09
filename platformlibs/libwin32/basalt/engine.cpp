#include <basalt/api/engine.h>

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

auto quit() -> void {
  PostQuitMessage(0);
}

} // namespace basalt
