#ifndef SERVER_CURSOR_H
#define SERVER_CURSOR_H

#include <wlr/types/wlr_cursor.h>

enum server_cursor_mode
{
  SERVER_CURSOR_MODE_PASSTHROUGH,
  SERVER_CURSOR_MODE_MOVE,
  SERVER_CURSOR_MODE_RESIZE,
};

extern struct wlr_cursor* server_cursor;
extern enum server_cursor_mode server_cursor_mode;

void
server_cursor_init();

#endif
