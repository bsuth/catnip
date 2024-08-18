#ifndef CATNIP_EXTENSIONS_WAYLAND_H
#define CATNIP_EXTENSIONS_WAYLAND_H

#include <wayland-server-core.h>

void
wl_signal_subscribe(
  struct wl_signal* signal,
  struct wl_listener* listener,
  wl_notify_func_t callback
);

#endif
