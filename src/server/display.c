#include "display.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wl_display* server_display;
const char* server_display_socket;

void
server_display_init()
{
  server_display = wl_display_create();

  server_display_socket = wl_display_add_socket_auto(server_display);
  if (!server_display_socket) {
    wlr_log(WLR_ERROR, "failed to create server_socket");
    exit(EXIT_FAILURE);
  }
}
