#ifndef CATNIP_BACKEND_H
#define CATNIP_BACKEND_H

#include <wlr/backend.h>

extern struct wlr_backend* catnip_backend;

void
catnip_backend_init();

#endif
