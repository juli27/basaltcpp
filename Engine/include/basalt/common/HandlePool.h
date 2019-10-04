#pragma once
#ifndef BS_COMMON_HANDLE_POOL_H
#define BS_COMMON_HANDLE_POOL_H

#include "Types.h"

#include <algorithm>
#include <tuple>
#include <vector>

namespace basalt {

template <typename T, typename HandleT>
struct HandlePool final {
  using ForEachFn = void (*) (T&);

  HandlePool() noexcept = default;
  HandlePool(const HandlePool&) = delete;
  HandlePool(HandlePool&&) noexcept = default;
  ~HandlePool() noexcept = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;
  // TODO: exception specification
  auto operator=(HandlePool&&) -> HandlePool& = default;

  [[nodiscard]] auto allocate() -> std::tuple<HandleT, T&>;
  void deallocate(HandleT handle);

  [[nodiscard]] auto get(HandleT handle) -> T&;

  void for_each(ForEachFn fn);

private:
  struct Slot {
    T mData = {};
    HandleT mHandle = {};
    HandleT mNextFreeSlot = {};
  };

  std::vector<Slot> mSlots;
  HandleT mFirstFreeSlot = {};
};

template <typename T, typename HandleT>
auto HandlePool<T, HandleT>::allocate() -> std::tuple<HandleT, T&> {
  if (mFirstFreeSlot) {
    Slot& slot = mSlots[mFirstFreeSlot.get_value()];
    slot.mHandle = mFirstFreeSlot;
    mFirstFreeSlot = slot.mNextFreeSlot;

    return {slot.mHandle, slot.mData};
  }

  const auto nextIndex = mSlots.size();
  constexpr auto maxSlots = static_cast<u32>(
    std::numeric_limits<typename HandleT::ValueT>::max()
  );

  if (nextIndex >= maxSlots) {
    throw std::out_of_range("out of slots");
  }

  const auto index = static_cast<typename HandleT::ValueT>(nextIndex);
  Slot& slot = mSlots.emplace_back();
  slot.mHandle = HandleT(index);

  return {slot.mHandle, slot.mData};
}

template <typename T, typename HandleT>
void HandlePool<T, HandleT>::deallocate(HandleT handle) {
  Slot& slot = mSlots.at(handle.get_value());
  slot.mHandle = HandleT{};
  slot.mNextFreeSlot = mFirstFreeSlot;
  mFirstFreeSlot = handle;
}

template <typename T, typename HandleT>
auto HandlePool<T, HandleT>::get(HandleT handle) -> T& {
  if (!handle) {
    throw std::runtime_error("invalid handle");
  }

  // throws exception with invalid index
  Slot& slot = mSlots.at(handle.get_value());
  return slot.mData;
}

template<typename T, typename HandleT>
void HandlePool<T, HandleT>::for_each(ForEachFn fn) {
  std::for_each(std::begin(mSlots), std::end(mSlots), [=](Slot& slot) {
    if (slot.mHandle) {
      fn(slot.mData);
    }
  });
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_POOL_H
