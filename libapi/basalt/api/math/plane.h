#pragma once

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt {

// plane in R^3 is defined by a point p and a normal vector n = (a,b,c) and can
// be described with n * (x - p) = 0 and with d = -(p * n) => x * n + d = 0
class Plane final {
public:
  [[nodiscard]]
  static auto from_general_form(f32 a, f32 b, f32 c, f32 d) -> Plane;

  [[nodiscard]]
  static auto from_point_normal(Vector3f32 const& p, Vector3f32 const& n)
    -> Plane;

  [[nodiscard]]
  static auto from_points(Vector3f32 const& p1, Vector3f32 const& p2,
                          Vector3f32 const& p3) -> Plane;

  [[nodiscard]] constexpr auto operator==(Plane const& rhs) const -> bool {
    return mNormal == rhs.mNormal && mD == rhs.mD;
  }

  [[nodiscard]] constexpr auto operator!=(Plane const& rhs) const -> bool {
    return !(*this == rhs);
  }

  [[nodiscard]]
  constexpr auto normal() const -> Vector3f32 const& {
    return mNormal;
  }

  [[nodiscard]]
  constexpr auto d() const -> f32 {
    return mD;
  }

  [[nodiscard]]
  constexpr auto dot_normal(Vector3f32 const& v) const -> f32 {
    return mNormal.dot(v);
  }

  [[nodiscard]]
  constexpr auto dot_coords(Vector3f32 const& v) const -> f32 {
    return dot_normal(v) + mD;
  }

  // sign-preserving. negative when behind plane
  [[nodiscard]]
  constexpr auto distance(Vector3f32 const& x) const -> f32 {
    return dot_coords(x);
  }

private:
  Vector3f32 mNormal;
  f32 mD{};

  constexpr Plane(Vector3f32 const& n, f32 const d) : mNormal{n}, mD{d} {
  }
};

} // namespace basalt
