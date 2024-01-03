#include "cursor.h"
#include "backend.h"
#include "config.h"
#include "cursor/lua_cursor.h"
#include "output/output_layout.h"
#include "scene.h"
#include "seat/seat.h"
#include "utils/wayland.h"
#include <string.h>

struct wlr_cursor* catnip_cursor = NULL;
struct wlr_xcursor_manager* catnip_xcursor_manager = NULL;
char* catnip_cursor_name = NULL;

static struct {
  struct wl_listener motion;
  struct wl_listener motion_absolute;
  struct wl_listener button;
  struct wl_listener axis;
  struct wl_listener frame;
  struct wl_listener new_input;
  struct wl_listener request_set_cursor;
} listeners;

static void
catnip_cursor_motion(struct wl_listener* listener, void* data)
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
catnip_cursor_motion_absolute(struct wl_listener* listener, void* data)
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
catnip_cursor_button(struct wl_listener* listener, void* data)
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
catnip_cursor_axis(struct wl_listener* listener, void* data)
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
catnip_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(catnip_seat);
}

static void
catnip_cursor_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_POINTER) {
    return;
  }

  wlr_cursor_attach_input_device(catnip_cursor, device);
}

static void
catnip_cursor_request_set_cursor(struct wl_listener* listener, void* data)
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
    &listeners.motion,
    &catnip_cursor->events.motion,
    catnip_cursor_motion
  );
  wl_setup_listener(
    &listeners.motion_absolute,
    &catnip_cursor->events.motion_absolute,
    catnip_cursor_motion_absolute
  );
  wl_setup_listener(
    &listeners.button,
    &catnip_cursor->events.button,
    catnip_cursor_button
  );
  wl_setup_listener(
    &listeners.axis,
    &catnip_cursor->events.axis,
    catnip_cursor_axis
  );
  wl_setup_listener(
    &listeners.frame,
    &catnip_cursor->events.frame,
    catnip_cursor_frame
  );
  wl_setup_listener(
    &listeners.new_input,
    &catnip_backend->events.new_input,
    catnip_cursor_new_input
  );
  wl_setup_listener(
    &listeners.request_set_cursor,
    &catnip_seat->events.request_set_cursor,
    catnip_cursor_request_set_cursor
  );
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
