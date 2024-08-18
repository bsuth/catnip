#ifndef CATNIP_COMPOSITOR_RENDERER_H
#define CATNIP_COMPOSITOR_RENDERER_H

#include <wlr/render/wlr_renderer.h>

extern struct wlr_renderer* catnip_renderer;

void
catnip_renderer_init();

#endif
