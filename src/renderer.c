#include "renderer.h"
#include "backend.h"
#include "display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_renderer* catnip_renderer;

void
catnip_renderer_init()
{
  catnip_renderer = wlr_renderer_autocreate(catnip_backend);
  if (catnip_renderer == NULL) {
    log_error("failed to create renderer");
    exit(1);
  }

  wlr_renderer_init_wl_display(catnip_renderer, catnip_display);
}
