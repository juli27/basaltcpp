#pragma once

#include "context.h"
#include "types.h"

#include "basalt/api/shared/unique_handle.h"

#include <utility>

namespace basalt::gfx {

class ContextResourceDeleter {
public:
  explicit ContextResourceDeleter(ContextPtr ctx) : mCtx{std::move(ctx)} {
  }

  template <typename Handle>
  auto operator()(Handle const handle) const noexcept -> void {
    mCtx->destroy(handle);
  }

private:
  ContextPtr mCtx;
};

} // namespace basalt::gfx
