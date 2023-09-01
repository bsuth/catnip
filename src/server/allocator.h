#ifndef BWC_SERVER_ALLOCATOR_H
#define BWC_SERVER_ALLOCATOR_H

#include <wlr/render/allocator.h>

extern struct wlr_allocator* bwc_allocator;

void
bwc_allocator_init();

#endif
