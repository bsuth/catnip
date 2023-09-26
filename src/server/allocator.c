#include "allocator.h"
#include "server/backend.h"
#include "server/renderer.h"
#include "utils/log.h"
#include <stdlib.h>

struct wlr_allocator* server_allocator;

void
init_server_allocator()
{
  server_allocator = wlr_allocator_autocreate(server_backend, server_renderer);
  if (server_allocator == NULL) {
    log_error("failed to create server_allocator");
    exit(1);
  }
}
