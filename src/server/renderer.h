#ifndef BWC_SERVER_RENDERER_H
#define BWC_SERVER_RENDERER_H

#include <wlr/render/wlr_renderer.h>

extern struct wlr_renderer* bwc_renderer;

void
bwc_renderer_init();

#endif
