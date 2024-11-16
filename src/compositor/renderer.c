#include "renderer.h"
#include "compositor/backend.h"
#include "compositor/display.h"
#include "log.h"
#include <stdlib.h>

struct wlr_renderer* catnip_renderer = NULL;

void
catnip_renderer_init()
{
  catnip_renderer = wlr_renderer_autocreate(catnip_backend);

  if (catnip_renderer == NULL) {
    catnip_log_error("failed to create renderer");
    exit(1);
  }

  wlr_renderer_init_wl_display(catnip_renderer, catnip_display);
}
