#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/utils.h>

namespace basalt::gfx {

constexpr auto to_d3d(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

inline auto to_d3d(const ImageFormat format) -> D3DFORMAT {
  static constexpr EnumArray<ImageFormat, D3DFORMAT, 10> TO_D3D {
    {ImageFormat::Unknown, D3DFMT_UNKNOWN},
    {ImageFormat::B5G6R5, D3DFMT_R5G6B5},
    {ImageFormat::B5G5R5X1, D3DFMT_X1R5G5B5},
    {ImageFormat::B5G5R5A1, D3DFMT_A1R5G5B5},
    {ImageFormat::B8G8R8X8, D3DFMT_X8R8G8B8},
    {ImageFormat::B8G8R8A8, D3DFMT_A8R8G8B8},
    {ImageFormat::B10G10R10A2, D3DFMT_A2R10G10B10},
    {ImageFormat::D16, D3DFMT_D16},
    {ImageFormat::D24X8, D3DFMT_D24X8},
    {ImageFormat::D24S8, D3DFMT_D24S8},
  };
  static_assert(TO_D3D.size() == IMAGE_FORMAT_COUNT);

  return TO_D3D[format];
}

inline auto to_d3d(const MultiSampleCount sampleCount) -> D3DMULTISAMPLE_TYPE {
  static constexpr EnumArray<MultiSampleCount, D3DMULTISAMPLE_TYPE, 4> TO_D3D {
    {MultiSampleCount::One, D3DMULTISAMPLE_NONE},
    {MultiSampleCount::Two, D3DMULTISAMPLE_2_SAMPLES},
    {MultiSampleCount::Four, D3DMULTISAMPLE_4_SAMPLES},
    {MultiSampleCount::Eight, D3DMULTISAMPLE_8_SAMPLES},
  };
  static_assert(TO_D3D.size() == MULTI_SAMPLE_COUNT_COUNT);

  return TO_D3D[sampleCount];
}

} // namespace basalt::gfx
