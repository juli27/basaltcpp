#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/gfx/backend/types.h>

#include <numeric>

auto basalt::gfx::get_vertex_size_in_bytes(VertexLayoutSpan const& layout)
  -> uDeviceSize {
  return std::accumulate(
    layout.begin(), layout.end(), uDeviceSize{0},
    [](uDeviceSize const size, VertexElement const attribute) {
      return size + get_vertex_attribute_size_in_bytes(attribute);
    });
}
