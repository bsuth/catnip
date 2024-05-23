#ifndef CATNIP_CURSOR_H
#define CATNIP_CURSOR_H

#include <stdint.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

extern struct wlr_cursor* catnip_cursor;
extern struct wlr_xcursor_manager* catnip_xcursor_manager;
extern char* catnip_cursor_name;

void
catnip_cursor_notify(uint32_t time_msec);

void
catnip_cursor_xcursor(const char* name, int size);

void
catnip_cursor_move(double x, double y);

#endif
