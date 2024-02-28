#ifndef CATNIP_EVENT_LOOP_H
#define CATNIP_EVENT_LOOP_H

#include <wayland-server-core.h>

extern struct wl_event_loop* catnip_event_loop;

void
catnip_event_loop_init();

#endif
