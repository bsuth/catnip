#ifndef BWC_SERVER_OUTPUTS_H
#define BWC_SERVER_OUTPUTS_H

#include <wlr/types/wlr_output_layout.h>

struct bwc_output
{
  struct wl_list link;
  struct wlr_output* wlr_output;
  struct wl_listener frame;
  struct wl_listener destroy;
};

extern struct wlr_output_layout* bwc_output_layout;

void
bwc_output_init();

#endif
