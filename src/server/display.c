#include "display.h"

struct wl_display* bwc_display;

void
bwc_display_init()
{
  bwc_display = wl_display_create();
}

void
bwc_display_teardown()
{
  wl_display_destroy_clients(bwc_display);
  wl_display_destroy(bwc_display);
}
