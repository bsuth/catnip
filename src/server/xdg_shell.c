#include "xdg_shell.h"
#include "server/display.h"

struct wlr_xdg_shell* server_xdg_shell;

void
server_xdg_shell_init()
{
  server_xdg_shell = wlr_xdg_shell_create(server_display, 3);
}
