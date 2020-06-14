#include <runtime/engine.h>

#include "shared/Windows_custom.h"

namespace basalt {

void quit() {
  ::PostQuitMessage(0);
}

} // namespace basalt
