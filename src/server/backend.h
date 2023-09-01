#ifndef SERVER_BACKEND_H
#define SERVER_BACKEND_H

#include <wlr/backend.h>

extern struct wlr_backend* server_backend;

void
server_backend_init();

#endif
