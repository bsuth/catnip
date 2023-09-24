#include "wayland.h"
#include <time.h>

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

uint32_t
wlr_get_time_msec()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return 1000 * now.tv_sec + now.tv_nsec / 1000000;
}
