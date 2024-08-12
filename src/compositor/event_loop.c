#include "event_loop.h"
#include "compositor/display.h"

struct wl_event_loop* catnip_event_loop = NULL;

void
catnip_event_loop_init()
{
  catnip_event_loop = wl_display_get_event_loop(catnip_display);
}
