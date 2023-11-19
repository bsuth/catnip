#ifndef CATNIP_CURSOR_H
#define CATNIP_CURSOR_H

#include <wlr/types/wlr_cursor.h>

enum catnip_cursor_mode {
  CATNIP_CURSOR_MODE_PASSTHROUGH,
  CATNIP_CURSOR_MODE_MOVE,
  CATNIP_CURSOR_MODE_RESIZE,
};

extern struct wlr_cursor* catnip_cursor;
extern enum catnip_cursor_mode catnip_cursor_mode;

void
catnip_cursor_init();

void
catnip_cursor_update(uint32_t time_msec);

#endif
