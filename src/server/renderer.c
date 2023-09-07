#include "renderer.h"
#include "backend.h"
#include "display.h"
#include <glib.h>

struct wlr_renderer* server_renderer;

void
server_renderer_init()
{
  server_renderer = wlr_renderer_autocreate(server_backend);

  if (server_renderer == NULL) {
    g_error("failed to create server_renderer");
  }

  wlr_renderer_init_wl_display(server_renderer, server_display);
}
