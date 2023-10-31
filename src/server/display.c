#include "display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wl_display* server_display;
const char* server_display_socket;

void
server_display_init()
{
  server_display = wl_display_create();

  server_display_socket = wl_display_add_socket_auto(server_display);
  if (!server_display_socket) {
    log_error("failed to create server_socket");
    exit(1);
  }
}
