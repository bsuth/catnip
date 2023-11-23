#include "xdg_shell.h"
#include "display.h"

struct wlr_xdg_shell* catnip_xdg_shell = NULL;

void
catnip_xdg_shell_init()
{
  catnip_xdg_shell = wlr_xdg_shell_create(catnip_display, 3);
}
