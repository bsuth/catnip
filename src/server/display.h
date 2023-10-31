#ifndef CATNIP_SERVER_DISPLAY_H
#define CATNIP_SERVER_DISPLAY_H

#include <wayland-server-core.h>

extern struct wl_display* server_display;
extern const char* server_display_socket;

void
server_display_init();

#endif
