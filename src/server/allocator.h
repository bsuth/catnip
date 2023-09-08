#ifndef SERVER_ALLOCATOR_H
#define SERVER_ALLOCATOR_H

#include <wlr/render/allocator.h>

extern struct wlr_allocator* server_allocator;

void
init_server_allocator();

#endif
