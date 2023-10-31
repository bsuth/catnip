#include "output_layout.h"
#include "server/cursor.h"
#include "server/output.h"
#include <stdio.h>

struct wlr_output_layout* server_output_layout;

// -----------------------------------------------------------------------------
// Server Output Layout
// -----------------------------------------------------------------------------

struct server_output*
server_output_layout_output_at(double x, double y)
{
  struct wlr_output* wlr_output =
    wlr_output_layout_output_at(server_output_layout, x, y);

  struct server_output* output;

  wl_list_for_each(output, &server_outputs, link)
  {
    if (output->wlr_output == wlr_output) {
      return output;
    }
  }

  return NULL;
}

struct server_output*
server_output_layout_get_focused_output()
{
  // TODO: maybe this is better to be the output of the currently focused window
  // and default to mouse if there are no windows?
  return server_output_layout_output_at(server_cursor->x, server_cursor->y);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_output_layout_init()
{
  server_output_layout = wlr_output_layout_create();
}
