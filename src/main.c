#include "server/allocator.h"
#include "server/backend.h"
#include "server/cursor.h"
#include "server/display.h"
#include "server/output.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "server/seat.h"
#include "server/xdg_shell.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_DEBUG, NULL);
  char* startup_cmd = NULL;

  int c;
  while ((c = getopt(argc, argv, "s:h")) != -1) {
    switch (c) {
      case 's':
        startup_cmd = optarg;
        break;
      default:
        printf("Usage: %s [-s startup command]\n", argv[0]);
        return 0;
    }
  }

  if (optind < argc) {
    // unrecognized arguments
    printf("Usage: %s [-s startup command]\n", argv[0]);
    return 0;
  }

  bwc_display_init();
  bwc_backend_init();
  bwc_renderer_init();
  bwc_allocator_init();
  bwc_output_init();
  bwc_scene_init();

  wlr_compositor_create(bwc_display, bwc_renderer);
  wlr_subcompositor_create(bwc_display);
  wlr_data_device_manager_create(bwc_display);

  bwc_xdg_shell_init();
  bwc_cursor_init();
  bwc_seat_init();

  /* Add a Unix socket to the Wayland display. */
  const char* socket = wl_display_add_socket_auto(bwc_display);
  if (!socket) {
    wlr_backend_destroy(bwc_backend);
    return 1;
  }

  /* Start the backend. This will enumerate outputs and inputs, become the DRM
   * master, etc */
  if (!wlr_backend_start(bwc_backend)) {
    wlr_backend_destroy(bwc_backend);
    wl_display_destroy(bwc_display);
    return 1;
  }

  /* Set the WAYLAND_DISPLAY environment variable to our socket and run the
   * startup command if requested. */
  setenv("WAYLAND_DISPLAY", socket, true);
  if (startup_cmd) {
    if (fork() == 0) {
      execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void*)NULL);
    }
  }

  /* Run the Wayland event loop. This does not return until you exit the
   * compositor. Starting the backend rigged up all of the necessary event
   * loop configuration to listen to libinput events, DRM events, generate
   * frame events at the refresh rate, and so on. */
  wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);
  wl_display_run(bwc_display);

  bwc_display_teardown();

  return 0;
}
