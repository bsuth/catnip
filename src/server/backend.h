#ifndef SERVER_BACKEND_H
#define SERVER_BACKEND_H

#include <wlr/backend.h>

extern struct wlr_backend* server_backend;

void
init_server_backend();

#endif
