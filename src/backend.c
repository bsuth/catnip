#include "backend.h"
#include "display.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_backend* catnip_backend = NULL;

void
catnip_backend_init()
{
  catnip_backend = wlr_backend_autocreate(catnip_display, NULL);
  if (catnip_backend == NULL) {
    log_error("failed to create backend");
    exit(1);
  }
}