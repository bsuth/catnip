#ifndef CATNIP_DISPLAY_H
#define CATNIP_DISPLAY_H

#include <wayland-server-core.h>

extern struct wl_display* catnip_display;
extern const char* catnip_display_socket;

void
catnip_display_init();

#endif
