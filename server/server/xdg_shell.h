#ifndef CATNIP_SERVER_XDG_SHELL_H
#define CATNIP_SERVER_XDG_SHELL_H

#include <wlr/types/wlr_xdg_shell.h>

extern struct wlr_xdg_shell* server_xdg_shell;

void
init_server_xdg_shell();

#endif