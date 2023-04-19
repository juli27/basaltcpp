#include <basalt/win32/shared/win32_gfx_factory.h>

#if BASALT_DEV_BUILD
#include <basalt/gfx/backend/validating_context.h>
#endif

namespace basalt::gfx {

auto Win32GfxFactory::create_device_and_context(
  const HWND window, const DeviceAndContextDesc& desc) const
  -> DeviceAndContext {
  DeviceAndContext res {do_create_device_and_context(window, desc)};

#if BASALT_DEV_BUILD
  res.context = ValidatingContext::wrap(res.context);
#endif

  return res;
}

} // namespace basalt::gfx
