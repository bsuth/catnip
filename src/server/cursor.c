#include "cursor.h"
#include "output.h"
#include "seat.h"
#include "utils/wayland.h"
#include <glib.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_xcursor_manager.h>

struct wlr_cursor* server_cursor;
enum server_cursor_mode server_cursor_mode;
static struct wlr_xcursor_manager* server_cursor_manager;

static struct wl_listener server_cursor_motion_listener;
static struct wl_listener server_cursor_motion_absolute_listener;
static struct wl_listener server_cursor_button_listener;
static struct wl_listener server_cursor_axis_listener;
static struct wl_listener server_cursor_frame_listener;

// -----------------------------------------------------------------------------
// Event Listeners
// -----------------------------------------------------------------------------

static void
server_cursor_motion(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    server_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  wlr_xcursor_manager_set_cursor_image(
    server_cursor_manager,
    "left_ptr",
    server_cursor
  );
}

static void
server_cursor_motion_absolute(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    server_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  wlr_xcursor_manager_set_cursor_image(
    server_cursor_manager,
    "left_ptr",
    server_cursor
  );

  // TODO: emit window enter / exit events?
  /* focus_toplevel_at(server_cursor->x, server_cursor->y); */
}

static void
server_cursor_button(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  wlr_seat_pointer_notify_button(
    server_seat,
    event->time_msec,
    event->button,
    event->state
  );

  if (event->state == WLR_BUTTON_RELEASED) {
    server_cursor_mode = SERVER_CURSOR_MODE_PASSTHROUGH;
  }
}

static void
server_cursor_axis(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_axis_event* event = data;

  wlr_seat_pointer_notify_axis(
    server_seat,
    event->time_msec,
    event->orientation,
    event->delta,
    event->delta_discrete,
    event->source
  );
}

static void
server_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(server_seat);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
init_server_cursor()
{
  server_cursor_mode = SERVER_CURSOR_MODE_PASSTHROUGH;

  server_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(server_cursor, server_output_layout);

  server_cursor_manager = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(server_cursor_manager, 1);

  wl_setup_listener(
    &server_cursor_motion_listener,
    &server_cursor->events.motion,
    server_cursor_motion
  );
  wl_setup_listener(
    &server_cursor_motion_absolute_listener,
    &server_cursor->events.motion_absolute,
    server_cursor_motion_absolute
  );
  wl_setup_listener(
    &server_cursor_button_listener,
    &server_cursor->events.button,
    server_cursor_button
  );
  wl_setup_listener(
    &server_cursor_axis_listener,
    &server_cursor->events.axis,
    server_cursor_axis
  );
  wl_setup_listener(
    &server_cursor_frame_listener,
    &server_cursor->events.frame,
    server_cursor_frame
  );
}
