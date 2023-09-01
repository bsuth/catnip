#include "backend.h"
#include "display.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wlr_backend* server_backend;

void
server_backend_init()
{
  server_backend = wlr_backend_autocreate(server_display);

  if (server_backend == NULL) {
    wlr_log(WLR_ERROR, "failed to create server_backend");
    exit(EXIT_FAILURE);
  }
}
