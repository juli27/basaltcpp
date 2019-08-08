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
    T data = {};
    HandleT handle = {};
    HandleT nextFreeSlot = {};
  };

public:
  HandlePool() = default;
  HandlePool(HandlePool&&) = default;
  ~HandlePool() = default;

  HandlePool(const HandlePool&) = delete;

public:
  HandleT Allocate();
  void Deallocate(HandleT handle);
  T& Get(HandleT handle);

  using ForEachFn = void (*) (T&);
  void ForEach(ForEachFn fn);

public:
  HandlePool& operator=(HandlePool&&) = default;

  HandlePool& operator=(const HandlePool&) = delete;

private:
  std::vector<Slot> m_slots;
  HandleT m_firstFreeSlot;
};


template <typename T, typename HandleT>
HandleT HandlePool<T, HandleT>::Allocate() {
  if (m_firstFreeSlot) {
    Slot& slot = m_slots.at(m_firstFreeSlot.GetValue());
    slot.handle = m_firstFreeSlot;
    m_firstFreeSlot = slot.nextFreeSlot;

    return slot.handle;
  }

  const auto nextIndex = m_slots.size();
  if (nextIndex > static_cast<u32>(std::numeric_limits<HandleT::ValueT>::max())) {
    throw std::out_of_range("out of slots");
  }

  const i32 index = static_cast<i32>(nextIndex);
  Slot& slot = m_slots.emplace_back();
  slot.handle = HandleT(index);

  return slot.handle;
}


template <typename T, typename HandleT>
void HandlePool<T, HandleT>::Deallocate(HandleT handle) {
  Slot& slot = m_slots.at(handle.GetValue());
  slot.handle = HandleT();
  slot.nextFreeSlot = m_firstFreeSlot;
  m_firstFreeSlot = handle;
}


template <typename T, typename HandleT>
T& HandlePool<T, HandleT>::Get(HandleT handle) {
  if (!handle) {
    throw std::runtime_error("invalid handle");
  }

  // throws exception with invalid index
  Slot& slot = m_slots.at(handle.GetValue());
  return slot.data;
}

template<typename T, typename HandleT>
void HandlePool<T, HandleT>::ForEach(ForEachFn fn) {
  std::for_each(std::begin(m_slots), std::end(m_slots), [=](Slot& slot){
    if (slot.handle) {
      fn(slot.data);
    }
  });
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_POOL_H
