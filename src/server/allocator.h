#ifndef CATNIP_SERVER_ALLOCATOR_H
#define CATNIP_SERVER_ALLOCATOR_H

#include <wlr/render/allocator.h>

extern struct wlr_allocator* server_allocator;

void
server_allocator_init();

#endif
