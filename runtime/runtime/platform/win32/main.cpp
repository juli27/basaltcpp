#include "runtime/platform/win32/app.h"

#include "runtime/shared/win32/util.h"
#include "runtime/shared/win32/Windows_custom.h"

#include "runtime/shared/Log.h"

#include <exception>
#include <string>

using std::exception;
using std::wstring;

using basalt::Log;

_Use_decl_annotations_
auto WINAPI wWinMain(
  const HINSTANCE hInstance, HINSTANCE, LPWSTR, const int nShowCmd) -> int try {
  Log::init();

  try {
    basalt::win32::run(hInstance, nShowCmd);
  } catch (const exception& ex) {
    BASALT_LOG_FATAL("unhandled exception: {}", ex.what());

    // rethrow to trigger the message box
    throw;
  }

  Log::shutdown();

  return 0;
} catch (const exception& ex) {
  wstring mbText = L"Unhandled exception: \r\n";
  mbText.append(basalt::win32::create_wide_from_utf8(ex.what()));
  ::MessageBoxW(
    nullptr, mbText.c_str(), L"Basalt Fatal Error"
  , MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

  return 0;
} catch (...) {
  ::MessageBoxW(
    nullptr, L"An unknown fatal error occurred!", L"Basalt Fatal Error"
  , MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

  return 0;
}
