#include "server/allocator.h"
#include "server/backend.h"
#include "server/cursor.h"
#include "server/display.h"
#include "server/output.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "server/seat.h"
#include "server/xdg_shell.h"
#include "user_config/user_config.h"
#include <glib.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_DEBUG, NULL);

  // TODO CLI (getopt)

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

  init_user_config();

  // This must be set AFTER `server_backend_init()`, since
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", server_display_socket, true);

  if (!wlr_backend_start(server_backend)) {
    g_critical("failed to start server_backend");
    exit(EXIT_FAILURE);
  }

  wl_display_run(server_display);

  return 0;
}
