#pragma once

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/base/types.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

namespace basalt {

template <typename T, typename Handle>
class HandlePool final {
  static_assert(std::is_base_of_v<detail::HandleBase, Handle>);

  using ActiveSlotsList = std::vector<Handle>;
  using IndexType = typename Handle::ValueType;
  static constexpr IndexType sInvalidIndex =
    std::numeric_limits<IndexType>::max();

  using Allocator = std::pmr::polymorphic_allocator<T>;
  using AllocatorTraits = std::allocator_traits<Allocator>;
  using Pointer = typename AllocatorTraits::pointer;

public:
  using iterator = typename ActiveSlotsList::iterator;

  HandlePool() noexcept = default;

  HandlePool(const HandlePool&) = delete;
  HandlePool(HandlePool&&) noexcept = default;

  ~HandlePool() noexcept = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;
  auto operator=(HandlePool&&) noexcept -> HandlePool& = default;

  [[nodiscard]] auto is_valid(const Handle handle) const noexcept -> bool {
    return is_allocated(handle) &&
           mBookkeeping[handle.value()].handle == handle;
  }

  // handle must be valid
  [[nodiscard]] auto operator[](const Handle handle) const noexcept
    -> const T& {
    BASALT_ASSERT(is_valid(handle));

    return *mBookkeeping[handle.value()].data;
  }

  template <typename... Args>
  [[nodiscard]] auto emplace(Args&&... args) -> Handle {
    return allocate(std::forward<Args>(args)...);
  }

  template <typename... Args>
  [[nodiscard]] auto allocate(Args&&... args) -> Handle {
    if (mFreeSlotIndex != sInvalidIndex) {
      SlotData& slot {mBookkeeping[mFreeSlotIndex]};

      AllocatorTraits::construct(mAllocator, slot.data,
                                 std::forward<Args>(args)...);

      slot.handle = Handle {mFreeSlotIndex};
      mFreeSlotIndex = std::exchange(slot.nextFreeSlot, sInvalidIndex);

      return slot.handle;
    }

    const uSize nextIndex {mBookkeeping.size()};
    BASALT_ASSERT(nextIndex < std::numeric_limits<IndexType>::max());

    const auto index {static_cast<IndexType>(nextIndex)};

    T* element {AllocatorTraits::allocate(mAllocator, 1)};
    AllocatorTraits::construct(mAllocator, element,
                               std::forward<Args>(args)...);

    SlotData& slot {mBookkeeping.emplace_back(SlotData {
      element,
      Handle {index},
      sInvalidIndex,
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

    SlotData& slot {mBookkeeping[index]};
    slot.handle = Handle {};

    AllocatorTraits::destroy(mAllocator, slot.data);

    // don't add to freelist if last element is de-allocated
    if (index == mBookkeeping.size() - 1) {
      AllocatorTraits::deallocate(mAllocator, slot.data, 1);

      mBookkeeping.pop_back();
    } else {
      slot.nextFreeSlot = std::exchange(mFreeSlotIndex, index);
    }
  }

  auto begin() -> iterator {
    return mActiveSlots.begin();
  }

  auto end() -> iterator {
    return mActiveSlots.end();
  }

private:
  struct SlotData final {
    T* data;
    Handle handle;
    IndexType nextFreeSlot;
  };

  using MemoryResource = std::pmr::unsynchronized_pool_resource;

  std::unique_ptr<MemoryResource> mMemory {
    std::make_unique<MemoryResource>(std::pmr::pool_options {0, sizeof(T)})};
  std::pmr::polymorphic_allocator<T> mAllocator {mMemory.get()};
  std::vector<SlotData> mBookkeeping;
  ActiveSlotsList mActiveSlots;
  IndexType mFreeSlotIndex {sInvalidIndex};

  // handle must be < size
  [[nodiscard]] auto is_allocated(const Handle handle) const noexcept -> bool {
    return handle.value() < mBookkeeping.size();
  }
};

} // namespace basalt
