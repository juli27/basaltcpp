#pragma once

#include "handle.h"

#include <utility>

namespace basalt {

template <typename Handle, typename Deleter>
class UniqueHandle {
public:
  UniqueHandle(Handle const handle, Deleter deleter)
    : mDeleter{std::move(deleter)}
    , mHandle{handle} {
  }

  UniqueHandle(UniqueHandle const&) = delete;

  UniqueHandle(UniqueHandle&& other) noexcept
    : mDeleter{other.mDeleter}
    , mHandle{std::exchange(other.mHandle, nullhdl)} {
  }

  ~UniqueHandle() noexcept {
    mDeleter(mHandle);
  }

  auto operator=(UniqueHandle const&) -> UniqueHandle& = delete;

  auto operator=(UniqueHandle&& other) -> UniqueHandle& {
    auto tmp = UniqueHandle{std::move(other)};

    using std::swap;
    swap(mDeleter, tmp.mDeleter);
    swap(mHandle, tmp.mHandle);

    return *this;
  }

  operator Handle() const {
    return mHandle;
  }

  auto handle() const noexcept -> Handle {
    return mHandle;
  }

  auto release() -> Handle {
    return std::exchange(mHandle, nullhdl);
  }

private:
  Deleter mDeleter;
  Handle mHandle;
};

} // namespace basalt
