#include "cursor.h"
#include "output.h"
#include "seat.h"
#include <glib.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_xcursor_manager.h>

struct wlr_cursor* server_cursor;
enum server_cursor_mode server_cursor_mode;
static struct wlr_xcursor_manager* server_cursor_manager;

// -----------------------------------------------------------------------------
// server_cursor_motion
// -----------------------------------------------------------------------------

static GPtrArray* server_cursor_motion_callbacks;
static struct wl_listener server_cursor_motion_listener;

static void
server_cursor_motion_notify(struct wl_listener* listener, void* data)
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

  for (int i = 0; i < server_cursor_motion_callbacks->len; ++i) {
    ((server_cursor_motion_callback
    ) server_cursor_motion_callbacks->pdata[i])(event);
  }
}

static void
init_server_cursor_motion()
{
  server_cursor_motion_callbacks = g_ptr_array_new();
  server_cursor_motion_listener.notify = server_cursor_motion_notify;
  wl_signal_add(&server_cursor->events.motion, &server_cursor_motion_listener);
}

void
add_server_cursor_motion_callback(server_cursor_motion_callback callback)
{
  g_ptr_array_add(server_cursor_motion_callbacks, callback);
}

// -----------------------------------------------------------------------------
// server_cursor_motion_absolute
// -----------------------------------------------------------------------------

static GPtrArray* server_cursor_motion_absolute_callbacks;
static struct wl_listener server_cursor_motion_absolute_listener;

static void
server_cursor_motion_absolute_notify(struct wl_listener* listener, void* data)
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

  for (int i = 0; i < server_cursor_motion_absolute_callbacks->len; ++i) {
    ((server_cursor_motion_absolute_callback
    ) server_cursor_motion_absolute_callbacks->pdata[i])(event);
  }
}

static void
init_server_cursor_motion_absolute()
{
  server_cursor_motion_absolute_callbacks = g_ptr_array_new();
  server_cursor_motion_absolute_listener.notify =
    server_cursor_motion_absolute_notify;
  wl_signal_add(
    &server_cursor->events.motion_absolute,
    &server_cursor_motion_absolute_listener
  );
}

void
add_server_cursor_motion_absolute_callback(
  server_cursor_motion_absolute_callback callback
)
{
  g_ptr_array_add(server_cursor_motion_absolute_callbacks, callback);
}

// -----------------------------------------------------------------------------
// server_cursor_button
// -----------------------------------------------------------------------------

static GPtrArray* server_cursor_button_callbacks;
static struct wl_listener server_cursor_button_listener;

static void
server_cursor_button_notify(struct wl_listener* listener, void* data)
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

  for (int i = 0; i < server_cursor_button_callbacks->len; ++i) {
    ((server_cursor_button_callback
    ) server_cursor_button_callbacks->pdata[i])(event);
  }
}

static void
init_server_cursor_button()
{
  server_cursor_button_callbacks = g_ptr_array_new();
  server_cursor_button_listener.notify = server_cursor_button_notify;
  wl_signal_add(&server_cursor->events.button, &server_cursor_button_listener);
}

void
add_server_cursor_button_callback(server_cursor_button_callback callback)
{
  g_ptr_array_add(server_cursor_button_callbacks, callback);
}

// -----------------------------------------------------------------------------
// server_cursor_axis
// -----------------------------------------------------------------------------

static GPtrArray* server_cursor_axis_callbacks;
static struct wl_listener server_cursor_axis_listener;

static void
server_cursor_axis_notify(struct wl_listener* listener, void* data)
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

  for (int i = 0; i < server_cursor_axis_callbacks->len; ++i) {
    ((server_cursor_axis_callback) server_cursor_axis_callbacks->pdata[i])(event
    );
  }
}

static void
init_server_cursor_axis()
{
  server_cursor_axis_callbacks = g_ptr_array_new();
  server_cursor_axis_listener.notify = server_cursor_axis_notify;
  wl_signal_add(&server_cursor->events.axis, &server_cursor_axis_listener);
}

void
add_server_cursor_axis_callback(server_cursor_axis_callback callback)
{
  g_ptr_array_add(server_cursor_axis_callbacks, callback);
}

// -----------------------------------------------------------------------------
// server_cursor_frame
// -----------------------------------------------------------------------------

static struct wl_listener server_cursor_frame_listener;

static void
server_cursor_frame_notify(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(server_seat);
}

static void
init_server_cursor_frame()
{
  server_cursor_frame_listener.notify = server_cursor_frame_notify;
  wl_signal_add(&server_cursor->events.frame, &server_cursor_frame_listener);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_server_cursor()
{
  server_cursor_mode = SERVER_CURSOR_MODE_PASSTHROUGH;

  server_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(server_cursor, server_output_layout);

  server_cursor_manager = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(server_cursor_manager, 1);

  init_server_cursor_motion();
  init_server_cursor_motion_absolute();
  init_server_cursor_button();
  init_server_cursor_axis();
  init_server_cursor_frame();
}
