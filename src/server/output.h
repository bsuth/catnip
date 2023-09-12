#ifndef SERVER_OUTPUT_H
#define SERVER_OUTPUT_H

#include <wlr/types/wlr_output_layout.h>

struct server_output {
  struct wl_list link; // TODO: remove this? not sure why we need this
  struct wlr_output* wlr_output;
  struct wl_listener frame;
  struct wl_listener destroy;
};

extern struct wlr_output_layout* server_output_layout;

void
init_server_output();

#endif
