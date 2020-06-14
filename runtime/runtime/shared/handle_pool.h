#pragma once

#include "asserts.h"
#include "types.h"

#include <limits>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace basalt {

template <typename T, typename HandleT>
struct HandlePool final {
  HandlePool() noexcept = default;

  HandlePool(const HandlePool&) = delete;
  HandlePool(HandlePool&&) = delete;

  ~HandlePool() noexcept = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;
  auto operator=(HandlePool&&) -> HandlePool& = delete;

  [[nodiscard]]
  auto allocate() -> std::tuple<HandleT, T&> {
    if (mFirstFreeSlot) {
      Slot& slot = mSlots[mFirstFreeSlot.value()];
      slot.handle = mFirstFreeSlot;
      mFirstFreeSlot = slot.nextFreeSlot;

      return {slot.handle, slot.data};
    }

    const auto nextIndex = mSlots.size();
    constexpr auto maxSlots = static_cast<u32>(
      std::numeric_limits<typename HandleT::ValueT>::max());

    if (nextIndex >= maxSlots) {
      throw std::out_of_range {"out of slots"};
    }

    const auto index = static_cast<typename HandleT::ValueT>(nextIndex);
    Slot& slot = mSlots.emplace_back();
    slot.handle = HandleT(index);

    return {slot.handle, slot.data};
  }

  void deallocate(HandleT handle) {
    Slot& slot = mSlots.at(handle.value());
    slot.data.~T();
    slot.handle = HandleT {};
    slot.nextFreeSlot = mFirstFreeSlot;
    mFirstFreeSlot = handle;
  }

  [[nodiscard]]
  auto get(HandleT handle) -> T& {
    BASALT_ASSERT(handle);

    // throws exception with invalid index
    Slot& slot = mSlots.at(handle.value());
    return slot.data;
  }

private:
  struct Slot final {
    T data {};
    HandleT handle {};
    HandleT nextFreeSlot {};
  };

  std::vector<Slot> mSlots {};
  HandleT mFirstFreeSlot {};
};

} // namespace basalt
