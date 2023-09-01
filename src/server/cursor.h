#ifndef BWC_SERVER_CURSOR_H
#define BWC_SERVER_CURSOR_H

#include <wlr/types/wlr_cursor.h>

enum bwc_cursor_mode
{
  BWC_CURSOR_PASSTHROUGH,
  BWC_CURSOR_MOVE,
  BWC_CURSOR_RESIZE,
};

extern struct wlr_cursor* bwc_cursor;
extern enum bwc_cursor_mode bwc_cursor_mode;

void
bwc_cursor_init();

#endif
