#include "config/config.h"
#include "meta.h"
#include "output/output.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/cursor.h"
#include "server/display.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "server/seat.h"
#include "server/server.h"
#include "server/xdg_shell.h"
#include "utils/log.h"
#include "window/window.h"
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  // TODO CLI (getopt)

  wlr_log_init(WLR_LOG_LEVEL, NULL);

  server_display_init();
  server_backend_init(); // must init after display
  server_xdg_shell_init(); // must init after display
  server_seat_init(); // must init after backend
  server_renderer_init(); // must init after backend
  catnip_output_init(); // must init after backend
  server_allocator_init(); // must init after renderer
  server_scene_init(); // must init after output
  server_cursor_init(); // must init after output
  catnip_window_init(); // must init after xdg_shell

  wlr_compositor_create(server_display, server_renderer);
  wlr_subcompositor_create(server_display);
  wlr_data_device_manager_create(server_display);

  config_init();

  // This must be set AFTER `init_server_backend()`, since internally
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", server_display_socket, true);

  if (!wlr_backend_start(server_backend)) {
    log_error("failed to start server_backend");
    exit(EXIT_FAILURE);
  }

  // Load config AFTER the backend has started. Otherwise, the Wayland objects
  // (outputs, windows, etc) will not have been initialized yet and the user
  // will be unable to access them from their config.
  config_load();

  wl_display_run(server_display);

  return 0;
}
