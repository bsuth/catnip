#ifndef CATNIP_BACKEND_H
#define CATNIP_BACKEND_H

#include <wlr/backend.h>
#include <wlr/backend/session.h>

extern struct wlr_backend* catnip_backend;
extern struct wlr_session* catnip_session;

void
catnip_backend_init();

#endif
