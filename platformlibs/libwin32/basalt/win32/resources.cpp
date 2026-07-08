#include "resources.h"

namespace basalt {

auto win32::load_big_icon(HINSTANCE const module, LPCWSTR const name,
                          UINT loadFlags) -> HICON {
  loadFlags |= LR_DEFAULTSIZE;
  auto const handle = LoadImageW(module, name, IMAGE_ICON, 0, 0, loadFlags);

  return static_cast<HICON>(handle);
}

auto win32::load_small_icon(HINSTANCE const module, LPCWSTR const name,
                            UINT const loadFlags) -> HICON {
  auto const width = GetSystemMetrics(SM_CXSMICON);
  auto const height = GetSystemMetrics(SM_CYSMICON);
  auto const handle =
    LoadImageW(module, name, IMAGE_ICON, width, height, loadFlags);

  return static_cast<HICON>(handle);
}

auto win32::load_cursor(HINSTANCE const module, LPCWSTR const name,
                        UINT loadFlags) -> HCURSOR {
  loadFlags |= LR_DEFAULTSIZE;
  auto const handle = LoadImageW(module, name, IMAGE_CURSOR, 0, 0, loadFlags);

  return static_cast<HCURSOR>(handle);
}

} // namespace basalt
