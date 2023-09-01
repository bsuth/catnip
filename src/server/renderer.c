#include "renderer.h"
#include "backend.h"
#include "display.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wlr_renderer* bwc_renderer;

void
bwc_renderer_init()
{
  bwc_renderer = wlr_renderer_autocreate(bwc_backend);

  if (bwc_renderer == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_renderer");
    exit(EXIT_FAILURE);
  }

  wlr_renderer_init_wl_display(bwc_renderer, bwc_display);
}
