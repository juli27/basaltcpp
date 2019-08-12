#pragma once
#ifndef BS_COMMON_HANDLE_POOL_H
#define BS_COMMON_HANDLE_POOL_H

#include <algorithm>
#include <vector>

namespace basalt {

template <typename T, typename HandleT>
class HandlePool final {
private:
  struct Slot {
    T mData = {};
    HandleT mHandle = {};
    HandleT mNextFreeSlot = {};
  };

public:
  HandlePool() = default;
  HandlePool(HandlePool&&) = default;
  ~HandlePool() = default;

  HandlePool(const HandlePool&) = delete;

public:
  auto Allocate() -> HandleT;
  void Deallocate(HandleT handle);
  auto Get(HandleT handle) -> T&;

  using ForEachFn = void (*) (T&);
  void ForEach(ForEachFn fn);

public:
  auto operator=(HandlePool&&) -> HandlePool& = default;

  auto operator=(const HandlePool&) -> HandlePool& = delete;

private:
  std::vector<Slot> mSlots;
  HandleT mFirstFreeSlot;
};


template <typename T, typename HandleT>
auto HandlePool<T, HandleT>::Allocate() -> HandleT {
  if (mFirstFreeSlot) {
    Slot& slot = mSlots.at(mFirstFreeSlot.GetValue());
    slot.mHandle = mFirstFreeSlot;
    mFirstFreeSlot = slot.mNextFreeSlot;

    return slot.mHandle;
  }

  const auto nextIndex = mSlots.size();
  if (nextIndex > static_cast<u32>(std::numeric_limits<HandleT::ValueT>::max())) {
    throw std::out_of_range("out of slots");
  }

  const auto index = static_cast<i32>(nextIndex);
  Slot& slot = mSlots.emplace_back();
  slot.mHandle = HandleT(index);

  return slot.mHandle;
}


template <typename T, typename HandleT>
void HandlePool<T, HandleT>::Deallocate(HandleT handle) {
  Slot& slot = mSlots.at(handle.GetValue());
  slot.mHandle = HandleT();
  slot.mNextFreeSlot = mFirstFreeSlot;
  mFirstFreeSlot = handle;
}


template <typename T, typename HandleT>
auto HandlePool<T, HandleT>::Get(HandleT handle) -> T& {
  if (!handle) {
    throw std::runtime_error("invalid handle");
  }

  // throws exception with invalid index
  Slot& slot = mSlots.at(handle.GetValue());
  return slot.mData;
}

template<typename T, typename HandleT>
void HandlePool<T, HandleT>::ForEach(ForEachFn fn) {
  std::for_each(std::begin(mSlots), std::end(mSlots), [=](Slot& slot){
    if (slot.mHandle) {
      fn(slot.mData);
    }
  });
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_POOL_H
