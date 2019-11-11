#include "Util.h"
#include "Win32Platform.h"

#include <basalt/Engine.h> // run

#include "Win32APIHeader.h"
#include <basalt/Log.h>
#include <basalt/common/Asserts.h>

// for CommandLineToArgvW
//#include <shellapi.h>

#include <fmt/format.h>

#include <memory> // make_unique
#include <stdexcept>
#include <string>

using std::exception;
using std::string;
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

[[nodiscard]]
auto create_platform_name_string() -> string {
  DWORD historical = 0u;
  const auto size = ::GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL,
    L"kernel32.dll", &historical);
  if (size == 0u) {
    BS_ERROR("{}", create_winapi_error_message(::GetLastError()));
    return {};
  }

  const auto buffer = std::make_unique<std::byte[]>(size);
  if (!::GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, L"kernel32.dll", 0u, size,
    buffer.get())) {
    return {};
  }

  VS_FIXEDFILEINFO* versionInfo = nullptr;
  UINT versionInfoSize = 0u;
  if (!::VerQueryValueW(buffer.get(), L"\\",
    reinterpret_cast<void**>(&versionInfo), &versionInfoSize)) {
    return {};
  }

  BS_ASSERT(versionInfoSize >= sizeof(VS_FIXEDFILEINFO), "");
  BS_ASSERT(versionInfo, "");

  BOOL isWow64 = FALSE;
  if (!::IsWow64Process(::GetCurrentProcess(), &isWow64)) {
    BS_INFO("{}", create_winapi_error_message(::GetLastError()));
  }

  return fmt::format("Windows API ({}.{}.{}{})",
    HIWORD(versionInfo->dwFileVersionMS), LOWORD(versionInfo->dwFileVersionMS),
    HIWORD(versionInfo->dwFileVersionLS), isWow64 ? " WOW64" : "");
}

} // namespace

_Use_decl_annotations_
auto CALLBACK wWinMain(HINSTANCE instance, HINSTANCE, WCHAR*, int showCommand)
-> int try {
  basalt::log::init();

  basalt::platform::sInstance = instance;
  basalt::platform::sShowCommand = showCommand;
  basalt::platform::sPlatformName = create_platform_name_string();

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
