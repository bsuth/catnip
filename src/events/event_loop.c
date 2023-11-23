#include "event_loop.h"
#include "display.h"

struct wl_event_loop* catnip_event_loop = NULL;

struct wl_event_source*
catnip_event_loop_once(wl_event_loop_idle_func_t callback, void* data)
{
  return wl_event_loop_add_idle(catnip_event_loop, callback, data);
}

void
catnip_event_loop_init()
{
  catnip_event_loop = wl_display_get_event_loop(catnip_display);
}
