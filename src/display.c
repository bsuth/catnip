#include "display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wl_display* catnip_display;
const char* catnip_display_socket;

void
catnip_display_init()
{
  catnip_display = wl_display_create();

  catnip_display_socket = wl_display_add_socket_auto(catnip_display);
  if (!catnip_display_socket) {
    log_error("failed to create wayland socket");
    exit(1);
  }
}
