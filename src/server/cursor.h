#ifndef SERVER_CURSOR_H
#define SERVER_CURSOR_H

#include <wlr/types/wlr_pointer.h>

enum server_cursor_mode {
  SERVER_CURSOR_MODE_PASSTHROUGH,
  SERVER_CURSOR_MODE_MOVE,
  SERVER_CURSOR_MODE_RESIZE,
};

extern struct wlr_cursor* server_cursor;
extern enum server_cursor_mode server_cursor_mode;

void
init_server_cursor();

// -----------------------------------------------------------------------------
// server_cursor_motion
// -----------------------------------------------------------------------------

typedef void (*server_cursor_motion_callback)(
  struct wlr_pointer_motion_event* event
);

void
add_server_cursor_motion_callback(server_cursor_motion_callback callback);

// -----------------------------------------------------------------------------
// server_cursor_motion_absolute
// -----------------------------------------------------------------------------

typedef void (*server_cursor_motion_absolute_callback)(
  struct wlr_pointer_motion_absolute_event* event
);

void
add_server_cursor_motion_absolute_callback(
  server_cursor_motion_absolute_callback callback
);

// -----------------------------------------------------------------------------
// server_cursor_button
// -----------------------------------------------------------------------------

typedef void (*server_cursor_button_callback)(
  struct wlr_pointer_button_event* event
);

void
add_server_cursor_button_callback(server_cursor_button_callback callback);

// -----------------------------------------------------------------------------
// server_cursor_axis
// -----------------------------------------------------------------------------

typedef void (*server_cursor_axis_callback)(struct wlr_pointer_axis_event* event
);

void
add_server_cursor_axis_callback(server_cursor_axis_callback callback);

#endif
