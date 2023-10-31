#ifndef CATNIP_SERVER_OUTPUT_LAYOUT_H
#define CATNIP_SERVER_OUTPUT_LAYOUT_H

#include <wlr/types/wlr_output_layout.h>

extern struct wlr_output_layout* server_output_layout;

// -----------------------------------------------------------------------------
// Server Output Layout
// -----------------------------------------------------------------------------

struct server_output*
server_output_layout_output_at(double x, double y);

struct server_output*
server_output_layout_get_focused_output();

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_output_layout_init();

#endif
