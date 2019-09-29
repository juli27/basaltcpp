#include <stdexcept>
#include <string>

#include <basalt/Engine.h> // Run, log

#include "Util.h"
#include "Win32APIHeader.h"
#include "Win32Platform.h"

#include <basalt/Log.h>

namespace {

using std::exception;
using std::wstring;

} // namespace

_Use_decl_annotations_ int CALLBACK wWinMain(
  HINSTANCE instance, HINSTANCE, WCHAR* commandLine, int showCommand
) try {
  basalt::platform::init(instance, commandLine, showCommand);
  basalt::log::init();

  try {
    basalt::run();
  } catch (const exception& ex) {
    BS_FATAL("Unhandled exception: {}", ex.what());

    // written to log. now rethrow to trigger the message box
    throw;
  }

  return 0;
} catch (const exception& ex) {
  wstring mbText = L"Unhandled exception: \r\n";
  mbText.append(create_wide_from_utf8(ex.what()));
  ::MessageBoxW(
    nullptr, mbText.c_str(), L"Basalt Fatal Error",
    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
  );

  return 0;
} catch (...) {
  ::MessageBoxW(
    nullptr, L"An unknown fatal error occurred!", L"Basalt Fatal Error",
    MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
  );

  return 0;
}
