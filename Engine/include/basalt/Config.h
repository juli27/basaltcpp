#pragma once
#ifndef BS_CONFIG_H
#define BS_CONFIG_H

#include "gfx/Config.h"
#include "platform/Types.h"

namespace basalt {

struct Config final {
  WindowDesc mWindow;
  gfx::Config mGfx{};
};

} // namespace basalt

#endif // !BS_CONFIG_H
