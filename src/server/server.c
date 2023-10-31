#include "server.h"
#include "config/config.h"
#include "config/events.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/cursor.h"
#include "server/display.h"
#include "server/output.h"
#include "server/output_layout.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "server/seat.h"
#include "server/window.h"
#include "server/xdg_shell.h"
#include "utils/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>

// -----------------------------------------------------------------------------
// Server
// -----------------------------------------------------------------------------

void
server_start()
{
  if (!wlr_backend_start(server_backend)) {
    log_error("failed to start server_backend");
    exit(EXIT_FAILURE);
  }

  // Load config AFTER the backend has started. Otherwise, the Wayland objects
  // (outputs, windows, etc) will not have been initialized yet and the user
  // will be unable to access them from their config.
  config_load();

  wl_display_run(server_display);
}

void
server_stop()
{
  config_events_publish("quit");
  wl_display_terminate(server_display);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_init()
{
  server_display_init();
  server_backend_init();
  server_renderer_init();
  server_allocator_init();
  server_output_layout_init();
  server_output_init();
  server_scene_init();

  wlr_compositor_create(server_display, server_renderer);
  wlr_subcompositor_create(server_display);
  wlr_data_device_manager_create(server_display);

  server_xdg_shell_init();
  server_window_init();
  server_cursor_init();
  server_seat_init();

  // This must be set AFTER `init_server_backend()`, since internally
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", server_display_socket, true);
}
