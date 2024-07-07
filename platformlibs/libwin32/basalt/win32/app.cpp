#include "app.h"

#include "app_window.h"
#include "message_queue.h"
#include "util.h"

#include <basalt/dear_imgui.h>

#include <basalt/api/bootstrap.h>
#include <basalt/api/types.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/platform.h>

#include <array>
#include <chrono>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::chrono::duration;
using std::chrono::steady_clock;
using std::chrono::time_point;

namespace basalt {

namespace {

auto dump_config(Config const& config) -> void {
  BASALT_LOG_INFO("config"sv);
  BASALT_LOG_INFO("\truntime.debugUI.enabled = {}"sv,
                  config.get_bool("runtime.debugUI.enabled"s));
}

[[nodiscard]]
auto load_system_cursor(WCHAR const* id) noexcept -> HCURSOR {
  return static_cast<HCURSOR>(
    LoadImageW(nullptr, id, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
}

[[nodiscard]]
auto load_system_mouse_cursors() -> MouseCursors {
  return MouseCursors{
    {MouseCursor::Arrow, load_system_cursor(IDC_ARROW)},
    {MouseCursor::TextInput, load_system_cursor(IDC_IBEAM)},
    {MouseCursor::ResizeAll, load_system_cursor(IDC_SIZEALL)},
    {MouseCursor::ResizeNS, load_system_cursor(IDC_SIZENS)},
    {MouseCursor::ResizeEW, load_system_cursor(IDC_SIZEWE)},
    {MouseCursor::ResizeNESW, load_system_cursor(IDC_SIZENESW)},
    {MouseCursor::ResizeNWSE, load_system_cursor(IDC_SIZENWSE)},
    {MouseCursor::Hand, load_system_cursor(IDC_HAND)},
    {MouseCursor::NotAllowed, load_system_cursor(IDC_NO)},
  };
}

[[nodiscard]]
auto drain_message_queue(Win32MessageQueue const& messageQueue) -> bool {
  while (auto message = messageQueue.poll()) {
    if (message->message == WM_QUIT) {
      return false;
    }

    TranslateMessage(&*message);
    DispatchMessageW(&*message);
  }

  return true;
}

[[nodiscard]]
auto wait_for_messages(Win32MessageQueue const& messageQueue) -> bool {
  auto message = messageQueue.take();
  if (message.message == WM_QUIT) {
    return false;
  }

  TranslateMessage(&message);
  DispatchMessageW(&message);

  // handle any remaining messages in the queue
  return drain_message_queue(messageQueue);
}

[[nodiscard]]
auto run_lost_device_loop(Win32MessageQueue const& messageQueue,
                          gfx::Device& gfxDevice) -> bool {
  while (wait_for_messages(messageQueue)) {
    switch (gfxDevice.get_status()) {
    case gfx::DeviceStatus::Ok:
      return true;

    case gfx::DeviceStatus::Error:
      return false;

    case gfx::DeviceStatus::DeviceLost:
      break;

    case gfx::DeviceStatus::ResetNeeded:
      gfxDevice.reset();

      return true;
    }
  }

  return false;
}

} // namespace

auto Win32App::init(HMODULE const moduleHandle, int const showCommand)
  -> Win32App {
  auto config = Config{
    {"debug.scene_inspector.visible"s, false},
    {"runtime.debugUI.enabled"s, false},
  };
  auto clientApp = bootstrap_app(config);
  dump_config(config);

  auto appWindow = Win32AppWindow::create(moduleHandle, showCommand, clientApp);
  // TODO: Hack! This doesn't belong here
  config.set_enum("window.mode"s, appWindow->mode());

  auto const& gfxContext = appWindow->gfx_context();

  auto runtime = Runtime{
    std::move(config),
    gfxContext,
    DearImGui::create(*gfxContext, appWindow->handle()),
  };

  appWindow->input_manager().set_overlay(runtime.dear_imgui());
  runtime.set_root(clientApp.createRootView(runtime));

  return Win32App{std::move(appWindow), std::move(runtime)};
}

Win32App::~Win32App() noexcept = default;

auto Win32App::run() -> void {
  using Clock = steady_clock;
  auto startTime = Clock::now();
  auto deltaTime = SecondsF32{0s};

  while (drain_message_queue(*mMessageQueue)) {
    if (auto const mode =
          mRuntime.config().get_enum("window.mode"s, to_window_mode);
        mode != mAppWindow->mode()) {
      mAppWindow->set_mode(mode);
    }

    mAppWindow->input_manager().dispatch_pending(mRuntime.root());

    mRuntime.update({deltaTime});

    if (mRuntime.is_dirty()) {
      mRuntime.set_dirty(false);
      mAppWindow->set_mouse_cursor(mMouseCursors[mRuntime.mouse_cursor()]);
    }

    if (mAppWindow->present() == gfx::PresentResult::DeviceLost) {
      if (!run_lost_device_loop(*mMessageQueue,
                                *mRuntime.gfx_context().device())) {
        Platform::quit();
      }

      continue;
    }

    auto const endTime = Clock::now();
    deltaTime = endTime - startTime;
    startTime = endTime;
  }
}

Win32App::Win32App(Win32AppWindowPtr appWindow, Runtime runtime)
  : mMessageQueue{appWindow->message_queue()}
  , mMouseCursors{load_system_mouse_cursors()}
  , mAppWindow{std::move(appWindow)}
  , mRuntime{std::move(runtime)} {
  BASALT_ASSERT(mMessageQueue);
  BASALT_ASSERT(mAppWindow);
}

// namespace {
//
///**
// * \brief Processes the windows command line string and populates an argv
// *        style vector.
// *
// * No program name will be added to the array.
// *
// * \param commandLine the windows command line arguments.
// */
// void process_args(const WCHAR* commandLine) {
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
//
//} // namespace

} // namespace basalt
