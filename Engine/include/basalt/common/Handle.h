#pragma once
#ifndef BS_COMMON_HANDLE_H
#define BS_COMMON_HANDLE_H

#include "Types.h"

namespace basalt {


template <typename typeTag>
class Handle final {
public:
  using ValueT = i32;

public:
  constexpr Handle();
  constexpr Handle(ValueT value);
  constexpr Handle(const Handle<typeTag>&) = default;
  constexpr Handle(Handle<typeTag>&&) = default;
  inline ~Handle() = default;

  constexpr bool IsValid() const;
  constexpr ValueT GetValue() const;

  inline Handle<typeTag>& operator=(const Handle<typeTag>&) = default;
  inline Handle<typeTag>& operator=(Handle<typeTag>&&) = default;

private:
  ValueT m_value;
};


template <typename typeTag>
constexpr Handle<typeTag>::Handle() : m_value(0xFFFFFFFF) {}


template <typename typeTag>
constexpr Handle<typeTag>::Handle(ValueT value) : m_value(value) {}


template <typename typeTag>
constexpr bool Handle<typeTag>::IsValid() const {
  return !(0x80000000 & m_value);
}


template <typename typeTag>
constexpr typename Handle<typeTag>::ValueT Handle<typeTag>::GetValue() const {
  return m_value;
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_H
