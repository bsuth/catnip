#include "output_layout.h"
#include "cursor/cursor.h"

struct wlr_output_layout* catnip_output_layout = NULL;

struct catnip_output*
catnip_output_layout_output_at(double x, double y)
{
  struct wlr_output* wlr_output =
    wlr_output_layout_output_at(catnip_output_layout, x, y);

  struct catnip_output* output = NULL;

  wl_list_for_each(output, &catnip_outputs, link)
  {
    if (output->wlr_output == wlr_output) {
      return output;
    }
  }

  return NULL;
}

struct catnip_output*
catnip_output_layout_get_focused_output()
{
  // TODO: maybe this is better to be the output of the currently focused window
  // and default to mouse if there are no windows?
  return catnip_output_layout_output_at(catnip_cursor->x, catnip_cursor->y);
}

void
catnip_output_layout_init()
{
  catnip_output_layout = wlr_output_layout_create();
}
