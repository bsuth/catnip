#ifndef CATNIP_EVENT_LOOP_H
#define CATNIP_EVENT_LOOP_H

#include <wayland-server-core.h>

extern struct wl_event_loop* catnip_event_loop;

struct wl_event_source*
catnip_event_loop_once(wl_event_loop_idle_func_t callback, void* data);

void
catnip_event_loop_init();

#endif
