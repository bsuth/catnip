#include "display.h"
#include <glib.h>

struct wl_display* server_display;
const char* server_display_socket;

void
init_server_display()
{
  server_display = wl_display_create();

  server_display_socket = wl_display_add_socket_auto(server_display);
  if (!server_display_socket) {
    g_error("failed to create server_socket");
  }
}
