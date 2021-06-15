#include <basalt/win32/app.h>

#include <basalt/win32/shared/utils.h>
#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/api/shared/log.h>

#include <exception>
#include <string>

using std::exception;
using std::wstring;

using basalt::App;
using basalt::Log;

_Use_decl_annotations_ auto WINAPI wWinMain(const HINSTANCE hInstance,
                                            HINSTANCE, LPWSTR,
                                            const int nShowCmd) -> int try {
  Log::init();

  try {
    App::run(hInstance, nShowCmd);
  } catch (const exception& ex) {
    BASALT_LOG_FATAL("unhandled exception: {}", ex.what());
    Log::shutdown();

    // rethrow to trigger the message box
    throw;
  }

  Log::shutdown();

  return 0;
} catch (const exception& ex) {
  wstring mbText = L"Unhandled exception: \r\n";
  mbText.append(basalt::create_wide_from_utf8(ex.what()));
  ::MessageBoxW(nullptr, mbText.c_str(), L"Basalt Fatal Error",
                MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

  return 0;
} catch (...) {
  ::MessageBoxW(nullptr, L"An unknown fatal error occurred!",
                L"Basalt Fatal Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

  return 0;
}
