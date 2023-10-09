#include <basalt/api/math/plane.h>

namespace basalt {

// TODO: plane transformation

// TODO: replace dot_normal(), dot_point() with a better API
// for determining the location of a point relative to a plane
// * enum with inside, in front, behind values
// * angle of a normal vector relative to the plane

auto Plane::from_general_form(f32 const a, f32 const b, f32 const c,
                              f32 const d) -> Plane {
  auto const normal = Vector3f32{a, b, c};
  auto const normalLength = normal.length();

  return Plane{normal / normalLength, d / normalLength};
}

auto Plane::from_point_normal(Vector3f32 const& p, Vector3f32 const& n)
  -> Plane {
  return from_general_form(n.x(), n.y(), n.z(), -p.dot(n));
}

// TODO: review normal orientation
auto Plane::from_points(Vector3f32 const& p1, Vector3f32 const& p2,
                        Vector3f32 const& p3) -> Plane {
  return from_point_normal(p1, (p3 - p2).cross(p1 - p2));
}

} // namespace basalt
