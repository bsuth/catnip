#ifndef CATNIP_SERVER_CURSOR_H
#define CATNIP_SERVER_CURSOR_H

#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_pointer.h>

enum server_cursor_mode {
  SERVER_CURSOR_MODE_PASSTHROUGH,
  SERVER_CURSOR_MODE_MOVE,
  SERVER_CURSOR_MODE_RESIZE,
};

extern struct wlr_cursor* server_cursor;
extern enum server_cursor_mode server_cursor_mode;

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
server_cursor_get_lx();

int
server_cursor_get_ly();

int
server_cursor_get_gx();

int
server_cursor_get_gy();

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_cursor_set_lx(int new_lx);

void
server_cursor_set_ly(int new_ly);

void
server_cursor_set_gx(int new_gx);

void
server_cursor_set_gy(int new_gy);

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_cursor_init();

#endif
