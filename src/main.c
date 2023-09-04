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
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_DEBUG, NULL);

  // TODO CLI (getopt)

  server_display_init();
  server_backend_init();
  server_renderer_init();
  server_allocator_init();
  server_output_init();
  server_scene_init();

  wlr_compositor_create(server_display, server_renderer);
  wlr_subcompositor_create(server_display);
  wlr_data_device_manager_create(server_display);

  server_xdg_shell_init();
  server_cursor_init();
  server_seat_init();

  reload_user_config();

  // This must be set AFTER `server_backend_init()`, since
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", server_display_socket, true);

  if (!wlr_backend_start(server_backend)) {
    wlr_log(WLR_ERROR, "failed to start server_backend");
    exit(EXIT_FAILURE);
  }

  wl_display_run(server_display);

  return 0;
}
