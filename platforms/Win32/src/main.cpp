#include <stdexcept>

// include our windows header before spdlog in Log.h
#include "Win32APIHeader.h"

#include <basalt/Engine.h>
#include <basalt/Log.h>

#include "Win32Platform.h"

_Use_decl_annotations_ int CALLBACK wWinMain(
  HINSTANCE instance, HINSTANCE, WCHAR* commandLine, int showCommand
) try {
  basalt::platform::winapi::init(instance, commandLine, showCommand);
  basalt::log::Init();

  try {
    basalt::Run();
  } catch (const std::exception& ex) {
    BS_FATAL("Unhandled exception: {}", ex.what());

    // written to log. now rethrow to trigger the message box
    throw;
  }

  return 0;
} catch (const std::exception& ex) {
  std::wstring mbText = L"Unhandled exception: \r\n";
  mbText.append(
    basalt::platform::winapi::CreateWideFromUTF8(ex.what())
  );
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
