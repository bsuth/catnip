#ifndef CATNIP_COMPOSITOR_ALLOCATOR_H
#define CATNIP_COMPOSITOR_ALLOCATOR_H

#include <wlr/render/allocator.h>

extern struct wlr_allocator* catnip_allocator;

void
catnip_allocator_init();

#endif
