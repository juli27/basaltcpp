#pragma once

#include "types.h"

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <iterator>
#include <optional>
#include <utility>

namespace basalt::gfx {

template <VertexElement... Attr, uSize Num = sizeof...(Attr)>
constexpr auto make_vertex_layout() -> VertexLayoutArray<Num>;

// TODO: add runtime parsing
// template <typename InputIt>
// auto parse_vertex_layout(InputIt begin, InputIt end) -> VertexLayoutVector;

//template <typename Container>
//auto parse_vertex_layout(Container&&) -> VertexLayout<Container>;

enum class VertexElement : u8 {
  Position3F32,
  PositionTransformed4F32,
  Normal3F32,
  PointSize1F32,
  ColorDiffuse1U32A8R8G8B8,
  ColorSpecular1U32A8R8G8B8,
  TextureCoords1F32,
  TextureCoords2F32,
  TextureCoords3F32,
  TextureCoords4F32,
};

constexpr auto get_vertex_attribute_size_in_bytes(VertexElement const attribute)
  -> uDeviceSize {
  switch (attribute) {
  case VertexElement::ColorDiffuse1U32A8R8G8B8:
  case VertexElement::ColorSpecular1U32A8R8G8B8:
    return sizeof(u32);
  case VertexElement::PointSize1F32:
  case VertexElement::TextureCoords1F32:
    return sizeof(f32);

  case VertexElement::TextureCoords2F32:
    return 2 * sizeof(f32);

  case VertexElement::Position3F32:
  case VertexElement::Normal3F32:
  case VertexElement::TextureCoords3F32:
    return 3 * sizeof(f32);
  case VertexElement::PositionTransformed4F32:
  case VertexElement::TextureCoords4F32:
    return 4 * sizeof(f32);
  }

  BASALT_CRASH("illegal vertex element");
}

auto get_vertex_size_in_bytes(VertexLayoutSpan const&) -> uDeviceSize;

template <typename Container>
class VertexLayout {
public:
  using value_type = typename Container::value_type;

  static_assert(std::is_same_v<value_type, VertexElement>);

  template <typename... Attributes>
  static constexpr auto parse(Attributes... attributes)
    -> std::optional<VertexLayout> {
    auto container = Container{attributes...};

    using std::begin;
    using std::end;

    if (!is_valid(begin(container), end(container))) {
      return std::nullopt;
    }

    return VertexLayout{std::move(container)};
  }

  explicit constexpr VertexLayout(VertexLayoutSpan const& layout)
    : mAttributes(layout.begin(), layout.end()) {
  }

  auto constexpr attributes() const -> Container const& {
    return mAttributes;
  }

private:
  Container mAttributes;

  explicit constexpr VertexLayout(Container attributes)
    : mAttributes(std::move(attributes)) {
  }

  template <typename InputIt>
  static constexpr auto is_valid(InputIt first, InputIt last) -> bool {
    auto it = first;

    auto const specular = [&] {
      auto numTexCoords = 0;
      while (it != last) {
        if (numTexCoords >= 8) {
          return false;
        }

        switch (*it) {
        case VertexElement::TextureCoords1F32:
        case VertexElement::TextureCoords2F32:
        case VertexElement::TextureCoords3F32:
        case VertexElement::TextureCoords4F32:
          numTexCoords++;
          it++;
          continue;

        default:
          return false;
        }
      }

      return true;
    };
    auto const diffuse = [&] {
      if (it == last) {
        return true;
      }

      if (auto const element = *it;
          element == VertexElement::ColorSpecular1U32A8R8G8B8) {
        it++;
      }

      return specular();
    };
    auto const pointSize = [&] {
      if (it == last) {
        return true;
      }

      if (auto const element = *it;
          element == VertexElement::ColorDiffuse1U32A8R8G8B8) {
        it++;
      }

      return diffuse();
    };
    auto const normal = [&] {
      if (it == last) {
        return true;
      }

      if (auto const element = *it; element == VertexElement::PointSize1F32) {
        it++;
      }

      return pointSize();
    };
    auto const position = [&] {
      if (it == last) {
        return true;
      }

      if (auto const element = *it; element == VertexElement::Normal3F32) {
        it++;
      }

      return normal();
    };
    auto const layout = [&] {
      if (it == last) {
        return false;
      }

      switch (*it) {
      case VertexElement::Position3F32:
        it++;
        return position();
      case VertexElement::PositionTransformed4F32:
        it++;
        return normal();

      default:
        break;
      }

      return false;
    };

    return layout();
  }
};

} // namespace basalt::gfx

template <basalt::gfx::VertexElement... Attr, basalt::uSize Num>
constexpr auto basalt::gfx::make_vertex_layout() -> VertexLayoutArray<Num> {
  return VertexLayoutArray<Num>::parse(Attr...).value();
}
