#include "server.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/cursor.h"
#include "server/display.h"
#include "server/output.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "server/seat.h"
#include "server/xdg_shell.h"
#include "user_config/events.h"
#include "utils/log.h"
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>

void
init_server()
{
  init_server_display();
  init_server_backend();
  init_server_renderer();
  init_server_allocator();
  init_server_output();
  init_server_scene();

  wlr_compositor_create(server_display, server_renderer);
  wlr_subcompositor_create(server_display);
  wlr_data_device_manager_create(server_display);

  init_server_xdg_shell();
  init_server_cursor();
  init_server_seat();

  // This must be set AFTER `init_server_backend()`, since internally
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", server_display_socket, true);
}

void
start_server()
{
  if (!wlr_backend_start(server_backend)) {
    log_error("failed to start server_backend");
    exit(EXIT_FAILURE);
  }

  wl_display_run(server_display);
}

void
stop_server()
{
  wl_display_terminate(server_display);
  call_event_listeners("quit");
}
