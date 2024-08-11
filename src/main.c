#include "cli.h"
#include "config.h"
#include "lua_events.h"
#include "state/allocator.h"
#include "state/backend.h"
#include "state/cursor.h"
#include "state/display.h"
#include "state/event_loop.h"
#include "state/keyboards.h"
#include "state/output_layout.h"
#include "state/outputs.h"
#include "state/renderer.h"
#include "state/scene.h"
#include "state/seat.h"
#include "state/windows.h"
#include "state/xdg_shell.h"
#include "utils/log.h"
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
  catnip_xdg_shell_init(); // must init after display
  catnip_backend_init(); // must init after event loop
  catnip_seat_init(); // must init after backend
  catnip_renderer_init(); // must init after backend
  catnip_keyboards_init(); // must init after backend
  catnip_outputs_init(); // must init after backend
  catnip_allocator_init(); // must init after renderer
  catnip_windows_init(); // must init after xdg_shell + seat
  catnip_output_layout_init(); // must init after display + scene
  catnip_cursor_init(); // must init after output layout
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

    // Use a 1ms timeout to prevent the main loop from hogging the CPU. This is
    // the smallest timeout available and more than sufficient for freeing up
    // the CPU for other processes.
    if (wl_event_loop_dispatch(catnip_event_loop, 1) < 0) {
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
