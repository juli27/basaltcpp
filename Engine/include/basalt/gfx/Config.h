#pragma once
#ifndef BASALT_GFX_CONFIG_H
#define BASALT_GFX_CONFIG_H

#include <basalt/shared/Types.h>

namespace basalt::gfx {

enum class BackendApi : i8 {
  Default,
  D3D9
};


struct Config final {
  BackendApi mBackendApi = BackendApi::Default;
};

} // basalt::gfx

#endif // BASALT_GFX_CONFIG_H
