add_subdirectory("shared")

target_sources(LibWin32 PRIVATE
  "app.cpp"
  "app.h"
  "app_window.cpp"
  "app_window.h"
  "key_map.h"
  "message_queue.cpp"
  "message_queue.h"
  "types.h"
  "util.cpp"
  "util.h"
  "window.h"
  "window_class.h"
  "windows.cpp"
)
