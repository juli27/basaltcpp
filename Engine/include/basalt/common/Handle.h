#pragma once
#ifndef BS_COMMON_HANDLE_H
#define BS_COMMON_HANDLE_H

#include "Types.h"

namespace basalt {

enum class HandleTarget : i8 {
  GENERIC = 0,
  GFX_MESH,
  GFX_TEXTURE
};

template <HandleTarget typeTag = HandleTarget::GENERIC>
class Handle final {
public:
  using IndexT = i16;
  using GenT = i8;
  using ValueT = i32;

  static_assert(sizeof(IndexT) + sizeof(GenT) <= sizeof(ValueT));

public:
  constexpr Handle();
  constexpr Handle(IndexT index, GenT gen);
  constexpr Handle(const Handle<typeTag>&) = default;
  constexpr Handle(Handle<typeTag>&&) = default;
  inline ~Handle() = default;

  constexpr bool IsValid() const;
  constexpr IndexT GetIndex() const;
  constexpr GenT GetGen() const;

  inline Handle<typeTag>& operator=(const Handle<typeTag>&) = default;
  inline Handle<typeTag>& operator=(Handle<typeTag>&&) = default;

private:
  ValueT m_value;
};


template <HandleTarget typeTag>
constexpr Handle<typeTag>::Handle() : m_value(0xFFFFFFFF) {}


template <HandleTarget typeTag>
constexpr Handle<typeTag>::Handle(IndexT index, GenT gen) : m_value(index) {
  m_value |= (static_cast<i32>(gen) << 16);
}


template <HandleTarget typeTag>
constexpr bool Handle<typeTag>::IsValid() const {
  return !(0x80000000 & m_value);
}


template <HandleTarget typeTag>
constexpr typename Handle<typeTag>::IndexT Handle<typeTag>::GetIndex() const {
  return static_cast<IndexT> (m_value);
}


template <HandleTarget typeTag>
constexpr typename Handle<typeTag>::GenT Handle<typeTag>::GetGen() const {
  return static_cast<GenT>(m_value >> 16);
}

} // namespace basalt

#endif // !BS_COMMON_HANDLE_H
