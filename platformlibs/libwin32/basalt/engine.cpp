#include <basalt/api/engine.h>

#include <basalt/win32utilities/Windows_custom.h>

namespace basalt {

void quit() {
  ::PostQuitMessage(0);
}

} // namespace basalt
