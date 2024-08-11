#ifndef CATNIP_STATE_DISPLAY_H
#define CATNIP_STATE_DISPLAY_H

#include <wayland-server-core.h>

extern struct wl_display* catnip_display;
extern bool catnip_display_run;
extern const char* catnip_display_socket;

void
catnip_display_init();

#endif
