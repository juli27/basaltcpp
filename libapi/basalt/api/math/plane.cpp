#include <basalt/api/math/plane.h>

namespace basalt {

// TODO: plane transformation

// TODO: replace dot_normal(), dot_point() with a better API
// for determining the location of a point relative to a plane
// * enum with inside, in front, behind values
// * angle of a normal vector relative to the plane

auto Plane::from_general_form(const f32 a, const f32 b, const f32 c,
                              const f32 d) -> Plane {
  const Vector3f32 normal {a, b, c};
  const f32 normalLength {normal.length()};

  return Plane {normal / normalLength, d / normalLength};
}

auto Plane::from_point_normal(const Vector3f32& p, const Vector3f32& n)
  -> Plane {
  return from_general_form(n.x(), n.y(), n.z(), -p.dot(n));
}

// TODO: review normal orientation
auto Plane::from_points(const Vector3f32& p1, const Vector3f32& p2,
                        const Vector3f32& p3) -> Plane {
  return from_point_normal(p1, Vector3f32::cross(p3 - p2, p1 - p2));
}

} // namespace basalt
