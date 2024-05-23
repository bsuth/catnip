#include "outputs.h"

struct wl_list catnip_outputs;

void
catnip_outputs_init()
{
  wl_list_init(&catnip_outputs);
}
