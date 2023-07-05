#pragma once

#include <basalt/api/shared/asserts.h>

#include <basalt/api/base/types.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace basalt {

template <typename T, typename Handle>
struct HandlePool final {
private:
  using ActiveSlotsList = std::vector<Handle>;

public:
  using iterator = typename ActiveSlotsList::iterator;

  static_assert(std::is_base_of_v<detail::HandleBase, Handle>);

private:
  using IndexType = typename Handle::ValueType;

public:
  HandlePool() noexcept = default;

  HandlePool(const HandlePool&) = delete;
  HandlePool(HandlePool&&) noexcept = default;

  ~HandlePool() noexcept = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;
  auto operator=(HandlePool&&) noexcept -> HandlePool& = default;

private:
  // handle must be < size
  [[nodiscard]] auto is_allocated(const Handle handle) const noexcept -> bool {
    return mStorage[handle.value()].handle == handle;
  }

public:
  [[nodiscard]] auto is_valid(const Handle handle) const noexcept -> bool {
    return handle.value() < mStorage.size() && is_allocated(handle);
  }

  // handle must be valid
  [[nodiscard]] auto operator[](const Handle handle) const noexcept
    -> const T& {
    BASALT_ASSERT(is_valid(handle));

    return mStorage[handle.value()].data;
  }

  template <typename... Args>
  [[nodiscard]] auto allocate(Args&&... args) -> Handle {
    if (mFreeSlot) {
      SlotData& slot {mStorage[mFreeSlot.value()]};
      new (std::addressof(slot.data)) T {std::forward<Args>(args)...};
      slot.handle = mFreeSlot;
      mFreeSlot = slot.nextFreeSlot;

      return slot.handle;
    }

    const uSize nextIndex {mStorage.size()};
    BASALT_ASSERT(nextIndex < std::numeric_limits<IndexType>::max());

    const auto index {static_cast<IndexType>(nextIndex)};
    SlotData& slot {mStorage.emplace_back(SlotData {
      T {std::forward<Args>(args)...},
      Handle {index},
      Handle {},
    })};

    mActiveSlots.emplace_back(slot.handle);

    return slot.handle;
  }

  // ignores invalid handles
  auto deallocate(const Handle handle) noexcept -> void {
    if (!is_valid(handle)) {
      return;
    }

    mActiveSlots.erase(
      std::remove_if(mActiveSlots.begin(), mActiveSlots.end(),
                     [handle](const Handle& h) { return h == handle; }),
      mActiveSlots.end());

    const auto index {handle.value()};

    // don't add to freelist if last element is de-allocated
    if (index == mStorage.size() - 1) {
      mStorage.pop_back();

      return;
    }

    SlotData& slot {mStorage[index]};
    slot.data.~T();
    slot.handle = Handle {};
    slot.nextFreeSlot = mFreeSlot;
    mFreeSlot = handle;
  }

  auto begin() -> iterator {
    return mActiveSlots.begin();
  }

  auto end() -> iterator {
    return mActiveSlots.end();
  }

private:
  struct SlotData final {
    T data {};
    Handle handle;
    Handle nextFreeSlot;
  };

  std::vector<SlotData> mStorage;
  ActiveSlotsList mActiveSlots;
  Handle mFreeSlot;
};

} // namespace basalt
