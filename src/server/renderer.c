#include "renderer.h"
#include "backend.h"
#include "display.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wlr_renderer* server_renderer;

void
server_renderer_init()
{
  server_renderer = wlr_renderer_autocreate(server_backend);

  if (server_renderer == NULL) {
    wlr_log(WLR_ERROR, "failed to create server_renderer");
    exit(EXIT_FAILURE);
  }

  wlr_renderer_init_wl_display(server_renderer, server_display);
}
