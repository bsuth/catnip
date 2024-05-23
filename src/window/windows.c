#include "windows.h"

struct wl_list catnip_windows;

void
catnip_windows_init()
{
  wl_list_init(&catnip_windows);
}
