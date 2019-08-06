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
    T data{};
    HandleT handle;
    HandleT nextFreeSlot;
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

  using ForeachFn = void (*) (T&);
  void Foreach(ForeachFn fn);

public:
  HandlePool& operator=(const HandlePool&) = default;
  HandlePool& operator=(HandlePool&&) = default;

private:
  std::vector<Slot> m_slots;
  HandleT m_firstFreeSlot;
};


template <typename T, typename HandleT>
HandleT HandlePool<T, HandleT>::Allocate() {
  if (m_firstFreeSlot.IsValid()) {
    Slot& slot = m_slots.at(m_firstFreeSlot.GetValue());
    slot.handle = m_firstFreeSlot;
    m_firstFreeSlot = slot.nextFreeSlot;

    return slot.handle;
  }

  const auto nextIndex = m_slots.size();
  if (nextIndex > static_cast<u32>(std::numeric_limits<typename HandleT::IndexT>::max())) {
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
  if (!handle.IsValid()) {
    throw std::runtime_error("invalid handle");
  }

  // throws exception with invalid index
  Slot& slot = m_slots.at(handle.GetValue());
  return slot.data;
}

template<typename T, typename HandleT>
void HandlePool<T, HandleT>::Foreach(ForeachFn fn) {
  std::for_each(std::begin(m_slots), std::end(m_slots), [=](Slot& slot){
    if (slot.handle.IsValid()) {
      fn(slot.data);
    }
  });
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_POOL_H
