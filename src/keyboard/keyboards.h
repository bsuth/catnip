#ifndef CATNIP_KEYBOARDS_H
#define CATNIP_KEYBOARDS_H

#include <wayland-util.h>

extern struct wl_list catnip_keyboards;

void
catnip_keyboards_init();

#endif
