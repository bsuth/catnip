#include "keyboards.h"

struct wl_list catnip_keyboards;

void
catnip_keyboards_init()
{
  wl_list_init(&catnip_keyboards);
}
