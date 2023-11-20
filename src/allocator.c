#include "allocator.h"
#include "backend.h"
#include "renderer.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_allocator* catnip_allocator;

void
catnip_allocator_init()
{
  catnip_allocator = wlr_allocator_autocreate(catnip_backend, catnip_renderer);
  if (catnip_allocator == NULL) {
    log_error("failed to create allocator");
    exit(1);
  }
}
