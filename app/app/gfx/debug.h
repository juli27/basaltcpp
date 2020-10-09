#pragma once

#include "types.h"

namespace basalt::gfx {

struct Debug final {
  static void update(const AdapterInfo& currentAdapter);
};

} // namespace basalt::gfx
