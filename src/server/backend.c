#include "backend.h"
#include "display.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wlr_backend* bwc_backend;

void
bwc_backend_init()
{
  bwc_backend = wlr_backend_autocreate(bwc_display);

  if (bwc_display == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_backend");
    exit(EXIT_FAILURE);
  }
}
