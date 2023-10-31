#ifndef CATNIP_SERVER_LAYER_SHELL_H
#define CATNIP_SERVER_LAYER_SHELL_H

#include <wlr/types/wlr_layer_shell_v1.h>

extern struct wlr_layer_shell_v1* server_layer_shell;

void
server_layer_shell_init();

#endif
