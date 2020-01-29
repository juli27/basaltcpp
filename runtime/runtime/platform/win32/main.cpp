#include "runtime/Engine.h"

#include "runtime/platform/win32/Globals.h"

#include "runtime/shared/win32/Win32APIHeader.h"
#include "runtime/shared/win32/Win32SharedUtil.h"

// after windows.h
#include "runtime/shared/Log.h"

#include <stdexcept>
#include <string>

using std::exception;
using std::wstring;

namespace {

/**
 * \brief Processes the windows command line string and populates an argv
 *        style vector.
 *
 * No program name will be added to the array.
 *
 * \param commandLine the windows command line arguments.
 */
//void process_args(const WCHAR* commandLine) {
//  // check if the command line string is empty to avoid adding
//  // the program name to the argument vector
//  if (commandLine[0] == L'\0') {
//    return;
//  }
//
//  auto argc = 0;
//  auto** argv = ::CommandLineToArgvW(commandLine, &argc);
//  if (argv == nullptr) {
//    // no logging because the log might not be initialized yet
//    return;
//  }
//
//  sArgs.reserve(argc);
//  for (auto i = 0; i < argc; i++) {
//    sArgs.push_back(create_utf8_from_wide(argv[i]));
//  }
//
//  ::LocalFree(argv);
//}

} // namespace

_Use_decl_annotations_
auto CALLBACK wWinMain(HINSTANCE instance, HINSTANCE, WCHAR*, int showCommand)
-> int try {
  basalt::log::init();

  basalt::platform::sInstance = instance;
  basalt::platform::sShowCommand = showCommand;

  try {
    basalt::run();
  } catch (const exception& ex) {
    BASALT_LOG_FATAL("unhandled exception: {}", ex.what());

    // written to log. now rethrow to trigger the message box
    throw;
  }

  return 0;
} catch (const exception& ex) {
  wstring mbText = L"Unhandled exception: \r\n";
  mbText.append(basalt::create_wide_from_utf8(ex.what()));
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
