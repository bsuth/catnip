#include "init.h"
#include "backend.h"
#include "config.h"
#include "cursor/cursor.h"
#include "cursor/lua_cursor.h"
#include "output/output_layout.h"
#include "seat.h"
#include "utils/wayland.h"
#include <string.h>

static struct wl_listener cursor_motion_listener;
static struct wl_listener cursor_motion_absolute_listener;
static struct wl_listener cursor_button_listener;
static struct wl_listener cursor_axis_listener;
static struct wl_listener cursor_frame_listener;
static struct wl_listener new_input_listener;
static struct wl_listener request_set_cursor_listener;

static void
on_cursor_motion(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    catnip_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  catnip_cursor_notify(event->time_msec);
}

static void
on_cursor_motion_absolute(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    catnip_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  catnip_cursor_notify(event->time_msec);
}

static void
on_cursor_button(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  wlr_seat_pointer_notify_button(
    catnip_seat,
    event->time_msec,
    event->button,
    event->state
  );

  lua_catnip_cursor_publish_button_event(catnip_L, event);
}

static void
on_cursor_axis(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_axis_event* event = data;

  wlr_seat_pointer_notify_axis(
    catnip_seat,
    event->time_msec,
    event->orientation,
    event->delta,
    event->delta_discrete,
    event->source
  );
}

static void
on_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(catnip_seat);
}

static void
on_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_POINTER) {
    return;
  }

  wlr_cursor_attach_input_device(catnip_cursor, device);
}

static void
on_request_set_cursor(struct wl_listener* listener, void* data)
{
  struct wlr_seat_pointer_request_set_cursor_event* event = data;

  if (event->seat_client != catnip_seat->pointer_state.focused_client) {
    return; // only accept cursor requests from the currently focused client.
  }

  wlr_cursor_set_surface(
    catnip_cursor,
    event->surface,
    event->hotspot_x,
    event->hotspot_y
  );
}

void
catnip_cursor_init()
{
  catnip_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(catnip_cursor, catnip_output_layout);

  catnip_xcursor_manager = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(catnip_xcursor_manager, 1);

  catnip_cursor_name = strdup("default");
  wlr_cursor_set_xcursor(
    catnip_cursor,
    catnip_xcursor_manager,
    catnip_cursor_name
  );

  wl_setup_listener(
    &cursor_motion_listener,
    &catnip_cursor->events.motion,
    on_cursor_motion
  );
  wl_setup_listener(
    &cursor_motion_absolute_listener,
    &catnip_cursor->events.motion_absolute,
    on_cursor_motion_absolute
  );
  wl_setup_listener(
    &cursor_button_listener,
    &catnip_cursor->events.button,
    on_cursor_button
  );
  wl_setup_listener(
    &cursor_axis_listener,
    &catnip_cursor->events.axis,
    on_cursor_axis
  );
  wl_setup_listener(
    &cursor_frame_listener,
    &catnip_cursor->events.frame,
    on_cursor_frame
  );
  wl_setup_listener(
    &new_input_listener,
    &catnip_backend->events.new_input,
    on_new_input
  );
  wl_setup_listener(
    &request_set_cursor_listener,
    &catnip_seat->events.request_set_cursor,
    on_request_set_cursor
  );
}
