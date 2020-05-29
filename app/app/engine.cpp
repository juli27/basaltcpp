#include "runtime/Engine.h"

#include "shared/Windows_custom.h"

namespace basalt {

void quit() {
  ::PostQuitMessage(0);
}

} // namespace basalt
