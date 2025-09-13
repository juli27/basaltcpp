#pragma once

#include "handle.h"

#include "basalt/api/base/asserts.h"
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

  using Allocator = std::pmr::polymorphic_allocator<T>;
  using AllocatorTraits = std::allocator_traits<Allocator>;
  using Pointer = typename AllocatorTraits::pointer;

public:
  using iterator = typename ActiveSlotsList::iterator;
  using const_iterator = typename ActiveSlotsList::const_iterator;

  HandlePool() = default;

  HandlePool(HandlePool const&) = delete;
  HandlePool(HandlePool&&) noexcept = default;

  ~HandlePool() noexcept {
    for (auto const& handle : mActiveSlots) {
      AllocatorTraits::destroy(mAllocator, mSlots[handle.value()]);
    }
  }

  auto operator=(HandlePool const&) -> HandlePool& = delete;
  auto operator=(HandlePool&&) noexcept -> HandlePool& = default;

  [[nodiscard]]
  auto is_valid(Handle const handle) const noexcept -> bool {
    return is_allocated(handle) && is_active(handle);
  }

  // handle must be valid
  [[nodiscard]]
  auto operator[](Handle const handle) const noexcept -> T const& {
    BASALT_ASSERT(is_valid(handle));

    return *mSlots[handle.value()];
  }

  // handle must be valid
  [[nodiscard]]
  auto operator[](Handle const handle) noexcept -> T& {
    BASALT_ASSERT(is_valid(handle));

    return *mSlots[handle.value()];
  }

  template <typename... Args>
  [[nodiscard]]
  auto emplace(Args&&... args) -> Handle {
    auto* data = AllocatorTraits::allocate(mAllocator, 1);
    AllocatorTraits::construct(mAllocator, data, std::forward<Args>(args)...);

    if (!mFreeList.empty()) {
      auto const handle = mFreeList.back();
      mFreeList.pop_back();
      auto const index = handle.value();

      mSlots[index] = data;
      mActiveSlots.emplace_back(handle);

      return handle;
    }

    auto const handle = [&] {
      auto const index = mSlots.size();
      BASALT_ASSERT(index < std::numeric_limits<IndexType>::max());

      return Handle{static_cast<IndexType>(index)};
    }();

    mSlots.emplace_back(data);
    mActiveSlots.emplace_back(handle);

    return handle;
  }

  template <typename... Args>
  [[nodiscard]]
  auto emplace_at(Handle const hint, Args&&... args) -> Handle {
    if (is_valid(hint)) {
      return emplace(std::forward<Args>(args)...);
    }

    auto const index = hint.value();

    if (!is_allocated(hint)) {
      auto const oldSize = mSlots.size();
      mSlots.resize(index + 1);

      // add new unused slots to the free list
      auto const newSize = mSlots.size();
      for (auto i = oldSize; i < newSize - 1; i++) {
        mFreeList.push_back(Handle{static_cast<IndexType>(i)});
      }
    } else {
      mFreeList.erase(std::remove(mFreeList.begin(), mFreeList.end(), hint),
                      mFreeList.end());
    }

    auto* data = AllocatorTraits::allocate(mAllocator, 1);
    AllocatorTraits::construct(mAllocator, data, std::forward<Args>(args)...);

    mSlots[index] = data;
    mActiveSlots.emplace_back(hint);

    return hint;
  }

  auto destroy(Handle const handle) noexcept -> void {
    if (!is_valid(handle)) {
      return;
    }

    mActiveSlots.erase(
      std::remove(mActiveSlots.begin(), mActiveSlots.end(), handle),
      mActiveSlots.end());

    auto const index = handle.value();
    auto* data = mSlots[index];

    AllocatorTraits::destroy(mAllocator, data);
    AllocatorTraits::deallocate(mAllocator, data, 1);

    // don't add to freelist if last element is de-allocated
    if (index == mSlots.size() - 1) {
      mSlots.pop_back();
    } else {
      mFreeList.push_back(handle);
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
  using MemoryResource = std::pmr::unsynchronized_pool_resource;

  std::unique_ptr<MemoryResource> mMemory{
    std::make_unique<MemoryResource>(std::pmr::pool_options{0, sizeof(T)})};
  std::pmr::polymorphic_allocator<T> mAllocator{mMemory.get()};
  std::vector<T*> mSlots;
  ActiveSlotsList mActiveSlots;
  std::vector<Handle> mFreeList;

  [[nodiscard]]
  auto is_allocated(Handle const handle) const noexcept -> bool {
    return handle.value() < mSlots.size();
  }

  [[nodiscard]]
  auto is_active(Handle const handle) const noexcept -> bool {
    return std::find(mActiveSlots.begin(), mActiveSlots.end(), handle) !=
           mActiveSlots.end();
  }
};

} // namespace basalt
