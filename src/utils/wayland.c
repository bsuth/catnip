#include "wayland.h"

void
wl_setup_listener(
  struct wl_listener* listener,
  struct wl_signal* signal,
  wl_notify_func_t callback
)
{
  listener->notify = callback;
  wl_signal_add(signal, listener);
}
