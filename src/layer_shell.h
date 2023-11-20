#ifndef CATNIP_LAYER_SHELL_H
#define CATNIP_LAYER_SHELL_H

#include <wlr/types/wlr_layer_shell_v1.h>

extern struct wlr_layer_shell_v1* catnip_layer_shell;

void
catnip_layer_shell_init();

#endif
