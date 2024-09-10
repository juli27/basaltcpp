#pragma once

#include "asserts.h"
#include "handle.h"

#include "basalt/api/base/types.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

namespace basalt {

template <typename T, typename Handle>
class HandlePool {
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
  using const_iterator = typename ActiveSlotsList::const_iterator;

  HandlePool() = default;

  HandlePool(HandlePool const&) = delete;
  HandlePool(HandlePool&&) noexcept = default;

  ~HandlePool() noexcept = default;

  auto operator=(HandlePool const&) -> HandlePool& = delete;
  auto operator=(HandlePool&&) noexcept -> HandlePool& = default;

  [[nodiscard]]
  auto is_valid(Handle const handle) const noexcept -> bool {
    return is_allocated(handle) && is_active(handle);
  }

  // handle must be valid
  [[nodiscard]]
  auto
  operator[](Handle const handle) const noexcept -> T const& {
    BASALT_ASSERT(is_valid(handle));

    return *mBookkeeping[handle.value()].data;
  }

  // handle must be valid
  [[nodiscard]]
  auto
  operator[](Handle const handle) noexcept -> T& {
    BASALT_ASSERT(is_valid(handle));

    return *mBookkeeping[handle.value()].data;
  }

  template <typename... Args>
  [[nodiscard]]
  auto emplace(Args&&... args) -> Handle {
    return allocate(std::forward<Args>(args)...);
  }

  template <typename... Args>
  [[nodiscard]]
  auto allocate(Args&&... args) -> Handle {
    auto* data = AllocatorTraits::allocate(mAllocator, 1);
    AllocatorTraits::construct(mAllocator, data, std::forward<Args>(args)...);

    if (mFreeSlotIndex != sInvalidIndex) {
      auto const index = mFreeSlotIndex;
      auto& slot = mBookkeeping[index];
      mFreeSlotIndex = slot.nextFreeSlot;

      auto const handle = Handle{index};

      slot = SlotData{data, sInvalidIndex};
      mActiveSlots.emplace_back(handle);

      return handle;
    }

    auto const handle = [&] {
      auto const index = mBookkeeping.size();
      BASALT_ASSERT(index < std::numeric_limits<IndexType>::max());

      return Handle{static_cast<IndexType>(index)};
    }();

    mBookkeeping.emplace_back(SlotData{data, sInvalidIndex});
    mActiveSlots.emplace_back(handle);

    return handle;
  }

  template <typename... Args>
  [[nodiscard]]
  auto allocate(Handle const hint, Args&&... args) -> Handle {
    if (is_valid(hint)) {
      return allocate(std::forward<Args>(args)...);
    }

    auto const index = hint.value();
    auto& slot = [&]() -> SlotData& {
      if (!is_allocated(hint)) {
        auto const oldSize = mBookkeeping.size();
        mBookkeeping.resize(index + 1);

        // add new unused slots to the free list
        auto const newSize = mBookkeeping.size();
        for (auto i = oldSize; i < newSize - 1; i++) {
          auto& slot = mBookkeeping[i];
          slot.nextFreeSlot =
            std::exchange(mFreeSlotIndex, static_cast<IndexType>(i));
        };
      } else {
        // patch free list

        if (mFreeSlotIndex == index) {
          mFreeSlotIndex = std::exchange(
            mBookkeeping[mFreeSlotIndex].nextFreeSlot, sInvalidIndex);
        } else {
          for (auto freeSlotIndex = mBookkeeping[mFreeSlotIndex].nextFreeSlot;
               freeSlotIndex != sInvalidIndex;
               freeSlotIndex = mBookkeeping[freeSlotIndex].nextFreeSlot) {
            if (mBookkeeping[freeSlotIndex].nextFreeSlot == index) {
              mBookkeeping[freeSlotIndex].nextFreeSlot =
                std::exchange(mBookkeeping[index].nextFreeSlot, sInvalidIndex);
            }
          }
        }
      }

      return mBookkeeping[index];
    }();
    if (!slot.data) {
      slot.data = AllocatorTraits::allocate(mAllocator, 1);
    }

    auto const handle = Handle{index};

    AllocatorTraits::construct(mAllocator, slot.data,
                               std::forward<Args>(args)...);

    mActiveSlots.emplace_back(handle);

    return handle;
  }

  // ignores invalid handles
  auto deallocate(Handle const handle) noexcept -> void {
    if (!is_valid(handle)) {
      return;
    }

    mActiveSlots.erase(
      std::remove(mActiveSlots.begin(), mActiveSlots.end(), handle),
      mActiveSlots.end());

    auto const index = handle.value();
    auto& slot = mBookkeeping[index];
    auto* data = slot.data;

    AllocatorTraits::destroy(mAllocator, data);
    AllocatorTraits::deallocate(mAllocator, data, 1);

    // don't add to freelist if last element is de-allocated
    if (index == mBookkeeping.size() - 1) {
      mBookkeeping.pop_back();
    } else {
      slot = SlotData{};
      slot.nextFreeSlot = std::exchange(mFreeSlotIndex, index);
    }
  }

  [[nodiscard]]
  auto begin() -> iterator {
    return mActiveSlots.begin();
  }

  [[nodiscard]]
  auto begin() const -> const_iterator {
    return mActiveSlots.begin();
  }

  [[nodiscard]]
  auto end() -> iterator {
    return mActiveSlots.end();
  }

  [[nodiscard]]
  auto end() const -> const_iterator {
    return mActiveSlots.end();
  }

private:
  struct SlotData {
    T* data{};
    IndexType nextFreeSlot = sInvalidIndex;
  };

  using MemoryResource = std::pmr::unsynchronized_pool_resource;

  std::unique_ptr<MemoryResource> mMemory{
    std::make_unique<MemoryResource>(std::pmr::pool_options{0, sizeof(T)})};
  std::pmr::polymorphic_allocator<T> mAllocator{mMemory.get()};
  std::vector<SlotData> mBookkeeping;
  ActiveSlotsList mActiveSlots;
  IndexType mFreeSlotIndex{sInvalidIndex};

  [[nodiscard]]
  auto is_allocated(Handle const handle) const noexcept -> bool {
    return handle.value() < mBookkeeping.size();
  }

  [[nodiscard]]
  auto is_active(Handle const handle) const noexcept -> bool {
    return std::find(mActiveSlots.begin(), mActiveSlots.end(), handle) !=
           mActiveSlots.end();
  }
};

} // namespace basalt
