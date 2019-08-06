#pragma once
#ifndef BS_COMMON_HANDLE_H
#define BS_COMMON_HANDLE_H

#include "Types.h"

namespace basalt {


template <typename Tag>
class Handle final {
public:
  using IndexT = i32;
  using ValueT = i32;

  static_assert(sizeof(IndexT) <= sizeof(ValueT));

public:
  constexpr Handle();
  constexpr Handle(ValueT value);
  constexpr Handle(const Handle<Tag>&) = default;
  constexpr Handle(Handle<Tag>&&) = default;
  inline ~Handle() = default;

  constexpr bool IsValid() const;
  constexpr ValueT GetValue() const;

  inline Handle<Tag>& operator=(const Handle<Tag>&) = default;
  inline Handle<Tag>& operator=(Handle<Tag>&&) = default;

private:
  ValueT m_value;
};


template <typename Tag>
constexpr Handle<Tag>::Handle() : m_value(0xFFFFFFFF) {}


template <typename Tag>
constexpr Handle<Tag>::Handle(ValueT value) : m_value(value) {}


template <typename Tag>
constexpr bool Handle<Tag>::IsValid() const {
  return !(0x80000000 & m_value);
}


template <typename Tag>
constexpr typename Handle<Tag>::ValueT Handle<Tag>::GetValue() const {
  return m_value;
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_H
