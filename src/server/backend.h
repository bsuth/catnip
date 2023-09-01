#ifndef BWC_SERVER_BACKEND_H
#define BWC_SERVER_BACKEND_H

#include <wlr/backend.h>

extern struct wlr_backend* bwc_backend;

void
bwc_backend_init();

#endif
