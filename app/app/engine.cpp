#include "runtime/Engine.h"

#include "globals.h"
#include "runtime/gfx/types.h"

#include "shared/Windows_custom.h"

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
