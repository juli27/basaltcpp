#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace basalt {

template <typename Handle, typename Deleter>
class UniqueHandle {
  static_assert(std::is_trivially_copyable_v<Handle>);

public:
  UniqueHandle() = default;

  UniqueHandle(Handle const handle, Deleter deleter)
    : mDeleter{std::move(deleter)}
    , mHandle{handle} {
  }

  UniqueHandle(UniqueHandle const&) = delete;

  UniqueHandle(UniqueHandle&& other) noexcept : UniqueHandle{} {
    swap(*this, other);
  }

  ~UniqueHandle() noexcept {
    if (mHandle) {
      (*mDeleter)(mHandle);
    }
  }

  auto operator=(UniqueHandle other) noexcept -> UniqueHandle& {
    swap(*this, other);

    return *this;
  }

  operator Handle() const noexcept {
    return mHandle;
  }

  auto handle() const noexcept -> Handle {
    return mHandle;
  }

  auto release() noexcept -> Handle {
    auto const handle = mHandle;
    mHandle = nullhdl;
    mDeleter.reset();

    return handle;
  }

  friend auto swap(UniqueHandle& lhs, UniqueHandle& rhs) noexcept -> void {
    using std::swap;
    swap(lhs.mDeleter, rhs.mDeleter);
    swap(lhs.mHandle, rhs.mHandle);
  }

private:
  std::optional<Deleter> mDeleter;
  Handle mHandle;
};

} // namespace basalt
