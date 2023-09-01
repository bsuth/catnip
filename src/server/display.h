#ifndef BWC_SERVER_DISPLAY_H
#define BWC_SERVER_DISPLAY_H

#include <wayland-server-core.h>

extern struct wl_display* bwc_display;

void
bwc_display_init();
void
bwc_display_teardown();

#endif
