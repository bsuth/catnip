#include "backend.h"
#include "server/display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_backend* server_backend;

void
init_server_backend()
{
  server_backend = wlr_backend_autocreate(server_display);
  if (server_backend == NULL) {
    log_error("failed to create server_backend");
    exit(1);
  }
}
