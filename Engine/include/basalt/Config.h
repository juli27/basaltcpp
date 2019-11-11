#pragma once
#ifndef BASALT_CONFIG_H
#define BASALT_CONFIG_H

#include "gfx/Config.h"
#include "platform/Types.h"

namespace basalt {

struct Config final {
  WindowDesc mWindow;
  gfx::Config mGfx{};
};

} // namespace basalt

#endif // !BASALT_CONFIG_H
