#ifndef CATNIP_CURSOR_H
#define CATNIP_CURSOR_H

#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

extern struct wlr_cursor* catnip_cursor;
extern struct wlr_xcursor_manager* catnip_cursor_manager;

void
catnip_cursor_init();

void
catnip_cursor_update(uint32_t time_msec);

#endif
