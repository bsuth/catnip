#ifndef CATNIP_STATE_CURSOR_H
#define CATNIP_STATE_CURSOR_H

#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

extern struct wlr_cursor* catnip_cursor;
extern struct wlr_xcursor_manager* catnip_xcursor_manager;
extern char* catnip_xcursor_name;

void
catnip_cursor_init();

#endif
