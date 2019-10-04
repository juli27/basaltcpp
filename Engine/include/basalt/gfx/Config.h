#pragma once
#ifndef BS_GFX_CONFIG_H
#define BS_GFX_CONFIG_H

#include <basalt/common/Types.h>

namespace basalt::gfx {

enum class BackendApi : i8 {
  Default,
  D3D9
};


struct Config final {
  BackendApi mBackendApi = BackendApi::Default;
};

} // basalt::gfx

#endif // BS_GFX_CONFIG_H
