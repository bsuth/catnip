#include "backend.h"
#include "display.h"
#include <glib.h>

struct wlr_backend* server_backend;

void
init_server_backend()
{
  server_backend = wlr_backend_autocreate(server_display);
  if (server_backend == NULL) {
    g_error("failed to create server_backend");
  }
}
