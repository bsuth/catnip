#ifndef CATNIP_SERVER_RENDERER_H
#define CATNIP_SERVER_RENDERER_H

#include <wlr/render/wlr_renderer.h>

extern struct wlr_renderer* server_renderer;

void
init_server_renderer();

#endif
