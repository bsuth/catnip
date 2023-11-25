#include "allocator.h"
#include "backend.h"
#include "config/config.h"
#include "config/keybindings.h"
#include "cursor/cursor.h"
#include "display.h"
#include "events/event_loop.h"
#include "input/seat.h"
#include "meta.h"
#include "output/output.h"
#include "renderer.h"
#include "scene.h"
#include "utils/log.h"
#include "window/window.h"
#include "xdg_shell.h"
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  // TODO CLI (getopt)

  wlr_log_init(WLR_LOG_LEVEL, NULL);

  catnip_display_init();
  catnip_event_loop_init(); // must init after display
  catnip_backend_init(); // must init after display
  catnip_xdg_shell_init(); // must init after display
  catnip_seat_init(); // must init after backend
  catnip_renderer_init(); // must init after backend
  catnip_output_init(); // must init after backend
  catnip_allocator_init(); // must init after renderer
  catnip_scene_init(); // must init after output
  catnip_cursor_init(); // must init after output
  catnip_window_init(); // must init after xdg_shell

  wlr_compositor_create(catnip_display, catnip_renderer);
  wlr_subcompositor_create(catnip_display);
  wlr_data_device_manager_create(catnip_display);

  // TODO: remove these, should live entirely in Lua
  config_keybindings_init();

  // This must be set AFTER `catnip_backend_init()`, since internally
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", catnip_display_socket, true);

  if (!wlr_backend_start(catnip_backend)) {
    log_error("failed to start backend");
    exit(EXIT_FAILURE);
  }

  // Init config AFTER the backend has started. Otherwise, the Wayland objects
  // (outputs, windows, etc) will not have been initialized yet and the user
  // will be unable to access them from their config.
  //
  // TODO: allow CLI `-c, --config` option to set `catnip_config_path`
  catnip_config_path = NULL;
  catnip_config_init();

  wl_display_run(catnip_display);

  return 0;
}
