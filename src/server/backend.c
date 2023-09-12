#include "backend.h"
#include "display.h"
#include <glib.h>

struct wlr_backend* server_backend;

// -----------------------------------------------------------------------------
// server_backend_new_output
// -----------------------------------------------------------------------------

static GPtrArray* server_backend_new_output_callbacks;
static struct wl_listener server_backend_new_output_listener;

static void
server_backend_new_output_notify(struct wl_listener* listener, void* data)
{
  struct wlr_output* output = data;

  for (int i = 0; i < server_backend_new_output_callbacks->len; ++i) {
    ((server_backend_new_output_callback
    ) server_backend_new_output_callbacks->pdata[i])(output);
  }
}

static void
init_server_backend_new_output()
{
  server_backend_new_output_callbacks = g_ptr_array_new();
  server_backend_new_output_listener.notify = server_backend_new_output_notify;
  wl_signal_add(
    &server_backend->events.new_output,
    &server_backend_new_output_listener
  );
}

void
add_server_backend_new_output_callback(
  server_backend_new_output_callback callback
)
{
  g_ptr_array_add(server_backend_new_output_callbacks, callback);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_server_backend()
{
  server_backend = wlr_backend_autocreate(server_display);

  if (server_backend == NULL) {
    g_error("failed to create server_backend");
  }

  init_server_backend_new_output();
}
