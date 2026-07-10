#include "app.h"

#include "app_window.h"
#include "message_queue.h"
#include "resources.h"

#include "shared/types.h"
#include "shared/utils.h"

#include <basalt/dear_imgui.h>

#include <basalt/api/bootstrap.h>
#include <basalt/api/types.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/config.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>
#include <basalt/api/base/platform.h>

#include <imgui.h>

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;

namespace basalt {

namespace {

auto get_system_cursor(MouseCursor const mouseCursor) -> HCURSOR {
  auto const resourceName = [&] {
    switch (mouseCursor) {
    case MouseCursor::Arrow:
      return IDC_ARROW;
    case MouseCursor::TextInput:
      return IDC_IBEAM;
    case MouseCursor::ResizeAll:
      return IDC_SIZEALL;
    case MouseCursor::ResizeNS:
      return IDC_SIZENS;
    case MouseCursor::ResizeEW:
      return IDC_SIZEWE;
    case MouseCursor::ResizeNESW:
      return IDC_SIZENESW;
    case MouseCursor::ResizeNWSE:
      return IDC_SIZENWSE;
    case MouseCursor::Hand:
      return IDC_HAND;
    case MouseCursor::NotAllowed:
      return IDC_NO;
    case MouseCursor::Wait:
      return IDC_WAIT;
    case MouseCursor::Progress:
      return IDC_APPSTARTING;
    }
    BASALT_CRASH("unhandled MouseCursor value");
  }();

  return win32::load_cursor(nullptr, resourceName, LR_SHARED);
}

[[nodiscard]]
auto drain_message_queue(Win32MessageQueue& messageQueue) -> bool {
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
auto wait_for_messages(Win32MessageQueue& messageQueue) -> bool {
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
auto run_lost_device_loop(Win32MessageQueue& messageQueue,
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

auto get_default_gfx_context_info(gfx::AdapterInfos const& adapters)
  -> GfxContextCreateInfo {
  auto const& adapterInfo = adapters[0];
  auto const backBufferFormat = [&] {
    for (auto const& format : adapterInfo.sharedModeInfo.backBufferFormats) {
      if (format.renderTargetFormat == gfx::ImageFormat::B8G8R8A8) {
        return format;
      }
      if (format.renderTargetFormat == gfx::ImageFormat::B8G8R8X8) {
        return format;
      }
    }

    return adapterInfo.sharedModeInfo.backBufferFormats[0];
  }();

  return GfxContextCreateInfo{
    0,
    backBufferFormat.renderTargetFormat,
    backBufferFormat.depthStencilFormat,
    gfx::MultiSampleCount::One,
  };
}

} // namespace

auto Win32App::init(HMODULE const moduleHandle) -> Win32App {
  auto config = Config{};
  auto launchInfo = bootstrap_app(config);

  auto appWindow = [&] {
    Win32MessageQueue::ensure_for_current_thread();

    auto const& canvasInfo = launchInfo.canvasCreateInfo;
    auto gfxFactory = [&]() -> gfx::Win32GfxFactoryPtr {
      switch (canvasInfo.gfxBackendApi) {
      case gfx::BackendApi::Default:
      case gfx::BackendApi::Direct3D9:
        if (auto maybeFactory = gfx::D3D9Factory::create()) {
          return *std::move(maybeFactory);
        }
        break;
      }

      BASALT_CRASH("win32: no suitable graphics API available");
    }();

    auto const adapters = gfxFactory->enumerate_adapters();
    auto gfxContextInfo = canvasInfo.configureGfxContext
                            ? canvasInfo.configureGfxContext(adapters)
                            : get_default_gfx_context_info(adapters);
    auto const& adapterIdentifier = adapters[gfxContextInfo.adapter].identifier;
    BASALT_LOG_INFO("creating Direct3D9 context: adapter={}, driver={}",
                    adapterIdentifier.displayName,
                    adapterIdentifier.driverInfo);

    auto const title = create_wide_from_utf8(launchInfo.appName);

    return Win32AppWindow::create(moduleHandle, title, std::move(gfxFactory),
                                  gfxContextInfo, canvasInfo.size,
                                  canvasInfo.mode, canvasInfo.isUserResizeable);
  }();
  // TODO: Hack! This doesn't belong here
  config.set_enum("window.mode"s, appWindow->mode());

  auto const& gfxContext = appWindow->gfx_context();

  auto runtime = Runtime{std::move(config), gfxContext};

  appWindow->input_manager().set_overlay(runtime.dear_imgui());
  auto* imguiViewport = ImGui::GetMainViewport();
  imguiViewport->PlatformHandle = appWindow->handle();
  imguiViewport->PlatformHandleRaw = imguiViewport->PlatformHandle;

  runtime.set_root(launchInfo.createRootView(runtime));

  return Win32App{std::move(appWindow), std::move(runtime)};
}

Win32App::~Win32App() noexcept = default;

auto Win32App::run(int const showCommand) -> void {
  using Clock = std::chrono::steady_clock;
  auto startTime = Clock::now();
  auto deltaTime = SecondsF32{0s};

  mAppWindow->show(showCommand);

  auto& messageQueue = Win32MessageQueue::get_for_current_thread();
  while (drain_message_queue(messageQueue)) {
    if (auto const mode =
          mRuntime.config().get_enum("window.mode"s, to_window_mode);
        mode != mAppWindow->mode()) {
      mAppWindow->set_mode(mode);
    }

    mAppWindow->input_manager().dispatch_pending(mRuntime.root());

    mRuntime.update({deltaTime});

    if (mRuntime.is_dirty()) {
      mRuntime.set_dirty(false);
      mAppWindow->set_mouse_cursor(get_system_cursor(mRuntime.mouse_cursor()));
    }

    if (mAppWindow->present() == gfx::PresentResult::DeviceLost) {
      if (!run_lost_device_loop(messageQueue,
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
  : mAppWindow{std::move(appWindow)}
  , mRuntime{std::move(runtime)} {
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
