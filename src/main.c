#include "allocator.h"
#include "backend.h"
#include "cli.h"
#include "config.h"
#include "cursor/init.h"
#include "display.h"
#include "event_loop.h"
#include "keyboard/init.h"
#include "lua_events.h"
#include "output/init.h"
#include "renderer.h"
#include "scene.h"
#include "seat.h"
#include "utils/log.h"
#include "window/init.h"
#include "xdg_shell.h"
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

// Match latest from wlroots (see wlroots/types/wlr_compositor.c)
#define WLR_COMPOSITOR_VERSION 6

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_ERROR, NULL); // default log level

  catnip_cli_init(argc, argv);

  catnip_display_init();
  catnip_scene_init();
  catnip_event_loop_init(); // must init after display
  catnip_backend_init(); // must init after display
  catnip_xdg_shell_init(); // must init after display
  catnip_seat_init(); // must init after backend
  catnip_renderer_init(); // must init after backend
  catnip_keyboard_init(); // must init after backend
  catnip_allocator_init(); // must init after renderer
  catnip_window_init(); // must init after xdg_shell
  catnip_output_init(); // must init after backend + scene
  catnip_cursor_init(); // must init after output
  catnip_config_init(); // must init last

  wlr_compositor_create(
    catnip_display,
    WLR_COMPOSITOR_VERSION,
    catnip_renderer
  );

  wlr_subcompositor_create(catnip_display);
  wlr_data_device_manager_create(catnip_display);

  // This must be set AFTER `catnip_backend_init()`, since internally
  // `wlr_backend_autocreate()` checks this variable when creating the backend.
  setenv("WAYLAND_DISPLAY", catnip_display_socket, true);

  if (!wlr_backend_start(catnip_backend)) {
    log_error("failed to start backend");
    exit(EXIT_FAILURE);
  }

  while (catnip_display_run) {
    wl_display_flush_clients(catnip_display);

    if (wl_event_loop_dispatch(catnip_event_loop, 0) < 0) {
      break;
    }

    lua_catnip_events_publish(catnip_L, lua_catnip_subscriptions, "tick", 0);

    if (catnip_config_request_reload) {
      catnip_config_reload();
    }
  }

  lua_catnip_events_publish(catnip_L, lua_catnip_subscriptions, "quit", 0);
  lua_close(catnip_L);

  wl_display_terminate(catnip_display);

  return 0;
}
