#include "wayland.h"

void
wl_signal_subscribe(
  struct wl_signal* signal,
  struct wl_listener* listener,
  wl_notify_func_t callback
)
{
  listener->notify = callback;
  wl_signal_add(signal, listener);
}
