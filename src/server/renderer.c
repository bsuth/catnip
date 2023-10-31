#include "renderer.h"
#include "server/backend.h"
#include "server/display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_renderer* server_renderer;

void
server_renderer_init()
{
  server_renderer = wlr_renderer_autocreate(server_backend);
  if (server_renderer == NULL) {
    log_error("failed to create server_renderer");
    exit(1);
  }

  wlr_renderer_init_wl_display(server_renderer, server_display);
}
