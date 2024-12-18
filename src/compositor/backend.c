#include "backend.h"
#include "compositor/event_loop.h"
#include "log.h"
#include <stdlib.h>

struct wlr_backend* catnip_backend = NULL;
struct wlr_session* catnip_session = NULL;

void
catnip_backend_init()
{
  catnip_backend = wlr_backend_autocreate(catnip_event_loop, &catnip_session);

  if (catnip_backend == NULL) {
    catnip_log_error("failed to create backend");
    exit(1);
  }
}
