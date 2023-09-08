#ifndef SERVER_DISPLAY_H
#define SERVER_DISPLAY_H

#include <wayland-server-core.h>

extern struct wl_display* server_display;
extern const char* server_display_socket;

void
init_server_display();

#endif
