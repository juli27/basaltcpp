#pragma once

#include "vector_p.h"

#include <basalt/api/base/types.h>

#include <array>
#include <type_traits>

namespace basalt::detail {

struct MatrixIndex {
  constexpr MatrixIndex(uSize const aRowIdx, uSize const aColumnIdx)
    : rowIdx{aRowIdx}
    , columnIdx{aColumnIdx} {
  }

  uSize rowIdx{};
  uSize columnIdx{};
};

template <typename Derived, typename T, uSize Rows, uSize Columns>
class Matrix {
public:
  static constexpr auto NUM_ROWS = Rows;
  static constexpr auto NUM_COLUMNS = Columns;
  static constexpr auto NUM_ELEMENTS = Rows * Columns;

  static constexpr auto zero() -> Derived {
    return Derived{};
  }

  template <typename Other>
  static constexpr auto
  transposed(Matrix<Other, T, NUM_COLUMNS, NUM_ROWS> const& m) -> Derived {
    auto transposedM = Derived{};
    for (auto rowIdx = uSize{0}; rowIdx < NUM_ROWS; ++rowIdx) {
      for (auto columnIdx = uSize{0}; columnIdx < NUM_COLUMNS; ++columnIdx) {
        transposedM[{rowIdx, columnIdx}] = m[{columnIdx, rowIdx}];
      }
    }

    return transposedM;
  }

  constexpr auto mul(T const& scalar) -> Derived& {
    for (auto& element : mElements) {
      element *= scalar;
    }

    return self();
  }

  constexpr auto operator*=(T const& scalar) -> Derived& {
    return mul(scalar);
  }

  friend constexpr auto operator*(Derived m, T const& scalar) -> Derived {
    m *= scalar;
    return m;
  }

  friend constexpr auto operator*(T const& scalar, Derived const& m)
    -> Derived {
    return m * scalar;
  }

  template <typename DerivedVec,
            typename = std::enable_if_t<NUM_ROWS == NUM_COLUMNS>>
  friend constexpr auto operator*(Vector<DerivedVec, T, NUM_ROWS> const& v,
                                  Derived const& m) -> DerivedVec {
    auto result = DerivedVec{};
    for (auto columnIdx = uSize{0}; columnIdx < NUM_COLUMNS; ++columnIdx) {
      auto element = T{};
      for (auto i = uSize{0}; i < NUM_ROWS; ++i) {
        element += v[i] * m.mElements[element_idx(i, columnIdx)];
      }

      result[columnIdx] = element;
    }

    return result;
  }

  template <typename Lhs, typename Rhs, uSize N>
  friend constexpr auto operator*(Matrix<Lhs, T, NUM_ROWS, N> const& lhs,
                                  Matrix<Rhs, T, N, NUM_COLUMNS> const& rhs)
    -> Derived {
    auto result = Derived{};
    for (auto rowIdx = uSize{0}; rowIdx < NUM_ROWS; ++rowIdx) {
      for (auto columnIdx = uSize{0}; columnIdx < NUM_COLUMNS; ++columnIdx) {
        auto element = T{};
        for (auto i = uSize{0}; i < N; ++i) {
          element += lhs.mElements[element_idx(rowIdx, i)] *
                     rhs.mElements[element_idx(i, columnIdx)];
        }

        result.mElements[element_idx(rowIdx, columnIdx)] = element;
      }
    }

    return result;
  }

  constexpr auto div(T const& scalar) -> Derived& {
    for (auto& element : mElements) {
      element /= scalar;
    }

    return self();
  }

  constexpr auto operator/=(T const& scalar) -> Derived& {
    return div(scalar);
  }

  friend constexpr auto operator/(Derived m, T const& scalar) -> Derived {
    m /= scalar;
    return m;
  }

  friend constexpr auto operator==(Derived const& l, Derived const& r) -> bool {
    return l.mElements == r.mElements;
  }

  friend constexpr auto operator!=(Derived const& l, Derived const& r) -> bool {
    return !(l == r);
  }

  /*template <uSize Row, uSize Column,
            typename = std::enable_if_t<NUM_ROWS == NUM_COLUMNS>>
  constexpr auto minor() const -> T {
    static_assert(Row < NUM_ROWS && Column < NUM_COLUMNS,
                  "Matrix index out of bounds");
    if constexpr (NUM_ROWS == 3 && NUM_COLUMNS == 3) {
    }

    return std::get<element_idx(Row, Column)>(mElements);
  }*/

  template <uSize Row, uSize Column>
  constexpr auto get() const -> T const& {
    static_assert(Row < NUM_ROWS && Column < NUM_COLUMNS,
                  "Matrix index out of bounds");
    return std::get<element_idx(Row, Column)>(mElements);
  }

  template <uSize Row, uSize Column>
  constexpr auto get() -> T& {
    static_assert(Row < NUM_ROWS && Column < NUM_COLUMNS,
                  "Matrix index out of bounds");
    return std::get<element_idx(Row, Column)>(mElements);
  }

  constexpr auto operator[](MatrixIndex const idx) const -> T const& {
    return mElements[element_idx(idx.rowIdx, idx.columnIdx)];
  }

  constexpr auto operator[](MatrixIndex const idx) -> T& {
    return mElements[element_idx(idx.rowIdx, idx.columnIdx)];
  }

protected:
  constexpr auto self() const -> Derived const& {
    return static_cast<Derived const&>(*this);
  }

  constexpr auto self() -> Derived& {
    return static_cast<Derived&>(*this);
  }

private:
  friend Derived;

  constexpr Matrix() = default;

  template <typename... Elements>
  constexpr explicit Matrix(Elements... elements) : mElements{elements...} {
    static_assert(sizeof...(Elements) == NUM_ELEMENTS);
  }

  static constexpr auto element_idx(uSize const rowIdx, uSize const columnIdx)
    -> uSize {
    return rowIdx * NUM_COLUMNS + columnIdx;
  }

  std::array<T, NUM_ELEMENTS> mElements{};
};

} // namespace basalt::detail
