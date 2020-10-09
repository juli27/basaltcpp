#pragma once

#include "asserts.h"
#include "types.h"

#include <limits>
#include <tuple>
#include <type_traits>
#include <vector>

namespace basalt {

template <typename T, typename Handle>
struct HandlePool final {
  static_assert(std::is_base_of_v<detail::HandleBase, Handle>);

private:
  using IndexType = typename Handle::ValueType;

public:
  HandlePool() noexcept = default;

  HandlePool(const HandlePool&) = delete;
  HandlePool(HandlePool&&) noexcept = default;

  ~HandlePool() noexcept = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;
  auto operator=(HandlePool &&) -> HandlePool& = default;

private:
  // handle must be < size
  [[nodiscard]] auto is_slot_allocated(const Handle handle) const noexcept
    -> bool {
    return mStorage[handle.value()].handle == handle;
  }

public:
  [[nodiscard]] auto is_handle_valid(const Handle handle) const noexcept
    -> bool {
    return handle.value() < mStorage.size() && is_slot_allocated(handle);
  }

  // handle must be valid
  [[nodiscard]] auto operator[](const Handle handle) const noexcept
    -> const T& {
    BASALT_ASSERT(is_handle_valid(handle));

    return mStorage[handle.value()].data;
  }

  [[nodiscard]] auto allocate() -> std::tuple<Handle, T&> {
    if (mFreeSlot) {
      SlotData& slot = mStorage[mFreeSlot.value()];
      slot.handle = mFreeSlot;
      mFreeSlot = slot.nextFreeSlot;

      return {slot.handle, slot.data};
    }

    const uSize nextIndex = mStorage.size();
    BASALT_ASSERT(nextIndex < std::numeric_limits<IndexType>::max());

    const auto index = static_cast<IndexType>(nextIndex);
    SlotData& slot = mStorage.emplace_back();
    slot.handle = Handle {index};

    return {slot.handle, slot.data};
  }

  // ignores invalid handles
  void deallocate(const Handle handle) noexcept {
    if (!is_handle_valid(handle)) {
      return;
    }

    SlotData& slot = mStorage[handle.value()];
    slot.data.~T();
    slot.handle = Handle {};
    slot.nextFreeSlot = mFreeSlot;
    mFreeSlot = handle;
  }

private:
  struct SlotData final {
    T data {};
    Handle handle;
    Handle nextFreeSlot;
  };

  std::vector<SlotData> mStorage;
  Handle mFreeSlot;
};

} // namespace basalt
