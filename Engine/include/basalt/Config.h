#pragma once
#ifndef BS_CONFIG_H
#define BS_CONFIG_H

#include "platform/WindowTypes.h"

namespace basalt {

struct Config {
  WindowDesc mWindow;
  /* TODO:
   * ups: number of times the applications update method is called per second.
          for the future: split up physics, ai, input, etc.
   * target fps: upper bound for frames per second
  */
};

} // namespace basalt

#endif // !BS_CONFIG_H
