#ifndef CATNIP_UTILS_WAYLAND_H
#define CATNIP_UTILS_WAYLAND_H

#include <wayland-server-core.h>

void
wl_setup_listener(
  struct wl_listener* listener,
  struct wl_signal* signal,
  wl_notify_func_t callback
);

#endif
