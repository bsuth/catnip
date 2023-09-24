#include "server.h"
#include "server/server/allocator.h"
#include "server/server/backend.h"
#include "server/server/cursor.h"
#include "server/server/display.h"
#include "server/server/output.h"
#include "server/server/renderer.h"
#include "server/server/scene.h"
#include "server/server/seat.h"
#include "server/server/xdg_shell.h"
#include "server/user_config/events.h"
#include <glib.h>
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
    g_critical("failed to start server_backend");
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
