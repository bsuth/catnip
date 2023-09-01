#include "allocator.h"
#include "backend.h"
#include "display.h"
#include "renderer.h"
#include <stdlib.h>
#include <wlr/util/log.h>

struct wlr_allocator* bwc_allocator;

void
bwc_allocator_init()
{
  bwc_allocator = wlr_allocator_autocreate(bwc_backend, bwc_renderer);

  if (bwc_allocator == NULL) {
    wlr_log(WLR_ERROR, "failed to create wlr_allocator");
    exit(EXIT_FAILURE);
  }
}
