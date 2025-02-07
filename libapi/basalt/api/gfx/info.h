#pragma once

#include "types.h"
#include "backend/adapter.h"

#include "basalt/api/base/types.h"

namespace basalt::gfx {

struct Info {
  AdapterInfos adapterInfos;
  u32 currentAdapter;
  BackendApi backendApi;
};

} // namespace basalt::gfx
