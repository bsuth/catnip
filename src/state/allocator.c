#include "allocator.h"
#include "state/backend.h"
#include "state/renderer.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_allocator* catnip_allocator = NULL;

void
catnip_allocator_init()
{
  catnip_allocator = wlr_allocator_autocreate(catnip_backend, catnip_renderer);

  if (catnip_allocator == NULL) {
    log_error("failed to create allocator");
    exit(1);
  }
}
