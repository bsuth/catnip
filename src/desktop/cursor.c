#include "cursor.h"
#include "compositor/backend.h"
#include "compositor/scene.h"
#include "compositor/seat.h"
#include "config.h"
#include "desktop/cursor.h"
#include "desktop/lua_cursor.h"
#include "desktop/output.h"
#include "desktop/outputs.h"
#include "extensions/time.h"
#include "extensions/wayland.h"
#include <string.h>

struct wlr_cursor* catnip_cursor;
struct wlr_xcursor_manager* catnip_xcursor_manager;
char* catnip_xcursor_name;

static struct {
  struct wl_listener cursor_motion;
  struct wl_listener cursor_motion_absolute;
  struct wl_listener cursor_button;
  struct wl_listener cursor_axis;
  struct wl_listener cursor_frame;
  struct wl_listener backend_new_input;
  struct wl_listener seat_request_set_cursor;
} listeners;

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

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
    event->source,
    event->relative_direction
  );
}

static void
on_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(catnip_seat);
}

static void
on_backend_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_POINTER) {
    return;
  }

  wlr_cursor_attach_input_device(catnip_cursor, device);
}

static void
on_seat_request_set_cursor(struct wl_listener* listener, void* data)
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

  catnip_xcursor_name = strdup("default");
  wlr_cursor_set_xcursor(
    catnip_cursor,
    catnip_xcursor_manager,
    catnip_xcursor_name
  );

  wl_signal_subscribe(
    &catnip_cursor->events.motion,
    &listeners.cursor_motion,
    on_cursor_motion
  );
  wl_signal_subscribe(
    &catnip_cursor->events.motion_absolute,
    &listeners.cursor_motion_absolute,
    on_cursor_motion_absolute
  );
  wl_signal_subscribe(
    &catnip_cursor->events.button,
    &listeners.cursor_button,
    on_cursor_button
  );
  wl_signal_subscribe(
    &catnip_cursor->events.axis,
    &listeners.cursor_axis,
    on_cursor_axis
  );
  wl_signal_subscribe(
    &catnip_cursor->events.frame,
    &listeners.cursor_frame,
    on_cursor_frame
  );
  wl_signal_subscribe(
    &catnip_backend->events.new_input,
    &listeners.backend_new_input,
    on_backend_new_input
  );
  wl_signal_subscribe(
    &catnip_seat->events.request_set_cursor,
    &listeners.seat_request_set_cursor,
    on_seat_request_set_cursor
  );
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_cursor_xcursor(const char* name, int size)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(name, size);

  // Always load scale 1
  wlr_xcursor_manager_load(new_wlr_xcursor_manager, 1);

  // Ensure we have loaded a scaled theme for each output's scale
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    wlr_xcursor_manager_load(
      new_wlr_xcursor_manager,
      output->wlr.output->scale
    );
  }

  wlr_xcursor_manager_destroy(catnip_xcursor_manager);
  catnip_xcursor_manager = new_wlr_xcursor_manager;
}

void
catnip_cursor_notify(uint32_t time_msec)
{
  double sx = 0;
  double sy = 0;

  struct wlr_scene_node* target_node = wlr_scene_node_at(
    &catnip_scene->tree.node,
    catnip_cursor->x,
    catnip_cursor->y,
    &sx,
    &sy
  );

  if (target_node == NULL || target_node->type != WLR_SCENE_NODE_BUFFER) {
    wlr_seat_pointer_clear_focus(catnip_seat);
  } else {
    struct wlr_scene_buffer* target_buffer =
      wlr_scene_buffer_from_node(target_node);

    struct wlr_scene_surface* target_scene_surface =
      wlr_scene_surface_try_from_buffer(target_buffer);

    if (target_scene_surface != NULL) {
      wlr_seat_pointer_notify_enter(
        catnip_seat,
        target_scene_surface->surface,
        sx,
        sy
      );
    }

    wlr_seat_pointer_notify_motion(catnip_seat, time_msec, sx, sy);
  }
}

void
catnip_cursor_move(double x, double y)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, x, y);
  catnip_cursor_notify(get_time_msec());
}
