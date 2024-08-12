#ifndef CATNIP_COMPOSITOR_XDG_SHELL_H
#define CATNIP_COMPOSITOR_XDG_SHELL_H

#include <wlr/types/wlr_xdg_shell.h>

extern struct wlr_xdg_shell* catnip_xdg_shell;

void
catnip_xdg_shell_init();

#endif
