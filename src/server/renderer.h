#ifndef SERVER_RENDERER_H
#define SERVER_RENDERER_H

#include <wlr/render/wlr_renderer.h>

extern struct wlr_renderer* server_renderer;

void
server_renderer_init();

#endif
