#pragma once

#include "d3d9_custom.h"

#include <DxErr.h>

#include <string>
#include <system_error>

#if BASALT_DEBUG_BUILD

#define D3D9CHECK(x)                                                           \
  do {                                                                         \
    if (const HRESULT macroHr = (x); FAILED(macroHr)) {                        \
      DXTraceW(__FILE__, __LINE__, macroHr, nullptr, TRUE);                    \
    }                                                                          \
  } while (false)

#else // BASALT_DEBUG_BUILD

#define D3D9CHECK(x) (x)

#endif // !BASALT_DEBUG_BUILD

namespace basalt::gfx {

namespace detail {

class D3D9Category : public std::error_category {
public:
  constexpr D3D9Category() noexcept {
  }

  auto name() const noexcept -> const char* override {
    return "d3d9";
  }

  auto message(int condition) const -> std::string override {
    auto const errorDescription = DXGetErrorDescriptionA(condition);

    return std::string{errorDescription};
  }
};

} // namespace detail

inline auto d3d9_category() noexcept -> std::error_category const& {
  static auto sD3d9Category = detail::D3D9Category{};

  return sD3d9Category;
}

} // namespace basalt::gfx
