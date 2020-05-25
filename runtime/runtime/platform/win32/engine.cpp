#include "runtime/Engine.h"

#include "runtime/gfx/types.h"

#include "runtime/platform/win32/globals.h"
#include "runtime/shared/win32/Windows_custom.h"

namespace basalt {

using gfx::View;

using namespace win32;

void set_view(const View& view) {
  sCurrentView = view;
}

void quit() {
  ::PostQuitMessage(0);
}

} // namespace basalt
