#ifndef CATNIP_SERVER_OUTPUT_H
#define CATNIP_SERVER_OUTPUT_H

#include <wlr/types/wlr_output_layout.h>

extern struct wlr_output_layout* server_output_layout;

void
init_server_output();

#endif