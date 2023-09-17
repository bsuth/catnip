#include "xdg_shell.h"
#include "display.h"

struct wlr_xdg_shell* server_xdg_shell;

void
init_server_xdg_shell()
{
  server_xdg_shell = wlr_xdg_shell_create(server_display, 3);
}
