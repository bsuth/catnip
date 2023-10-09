#include "output_layout.h"

struct wlr_output_layout* server_output_layout;

void
init_server_output_layout()
{
  server_output_layout = wlr_output_layout_create();
}
