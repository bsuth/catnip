#ifndef SERVER_BACKEND_H
#define SERVER_BACKEND_H

#include <wlr/backend.h>
#include <wlr/types/wlr_output.h>

extern struct wlr_backend* server_backend;

void
init_server_backend();

// -----------------------------------------------------------------------------
// server_backend_new_output
// -----------------------------------------------------------------------------

typedef void (*server_backend_new_output_callback)(struct wlr_output* output);

void
add_server_backend_new_output_callback(
  server_backend_new_output_callback callback
);

#endif
