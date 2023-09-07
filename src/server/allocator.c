#include "allocator.h"
#include "backend.h"
#include "renderer.h"
#include <glib.h>

struct wlr_allocator* server_allocator;

void
server_allocator_init()
{
  server_allocator = wlr_allocator_autocreate(server_backend, server_renderer);

  if (server_allocator == NULL) {
    g_error("failed to create server_allocator");
  }
}
