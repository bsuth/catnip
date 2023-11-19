#include "cursor.h"
#include "output/output_layout.h"
#include "server/scene.h"
#include "server/seat.h"
#include "utils/time.h"
#include "utils/wayland.h"
#include <wlr/types/wlr_xcursor_manager.h>

struct wlr_cursor* catnip_cursor;
enum catnip_cursor_mode catnip_cursor_mode;
static struct wlr_xcursor_manager* catnip_cursor_manager;

static struct {
  struct wl_listener motion;
  struct wl_listener motion_absolute;
  struct wl_listener button;
  struct wl_listener axis;
  struct wl_listener frame;
} listeners;

static void
on_motion(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    catnip_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  catnip_cursor_update(event->time_msec);
}

static void
on_motion_absolute(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    catnip_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  catnip_cursor_update(event->time_msec);
}

static void
on_button(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  wlr_seat_pointer_notify_button(
    server_seat,
    event->time_msec,
    event->button,
    event->state
  );

  if (event->state == WLR_BUTTON_RELEASED) {
    catnip_cursor_mode = CATNIP_CURSOR_MODE_PASSTHROUGH;
  }
}

static void
on_axis(struct wl_listener* listener, void* data)
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
on_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(server_seat);
}

void
catnip_cursor_init()
{
  catnip_cursor_mode = CATNIP_CURSOR_MODE_PASSTHROUGH;

  catnip_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(catnip_cursor, catnip_output_layout);

  catnip_cursor_manager = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(catnip_cursor_manager, 1);

  wl_setup_listener(
    &listeners.motion,
    &catnip_cursor->events.motion,
    on_motion
  );
  wl_setup_listener(
    &listeners.motion_absolute,
    &catnip_cursor->events.motion_absolute,
    on_motion_absolute
  );
  wl_setup_listener(
    &listeners.button,
    &catnip_cursor->events.button,
    on_button
  );
  wl_setup_listener(&listeners.axis, &catnip_cursor->events.axis, on_axis);
  wl_setup_listener(&listeners.frame, &catnip_cursor->events.frame, on_frame);
}

void
catnip_cursor_update(uint32_t time_msec)
{
  double sx = 0;
  double sy = 0;

  struct wlr_scene_node* target_node = wlr_scene_node_at(
    &server_scene->tree.node,
    catnip_cursor->x,
    catnip_cursor->y,
    &sx,
    &sy
  );

  if (target_node == NULL || target_node->type != WLR_SCENE_NODE_BUFFER) {
    wlr_seat_pointer_clear_focus(server_seat);
    wlr_xcursor_manager_set_cursor_image(
      catnip_cursor_manager,
      "left_ptr",
      catnip_cursor
    );
  } else {
    struct wlr_scene_buffer* target_buffer =
      wlr_scene_buffer_from_node(target_node);

    struct wlr_scene_surface* target_scene_surface =
      wlr_scene_surface_from_buffer(target_buffer);

    if (target_scene_surface != NULL) {
      wlr_seat_pointer_notify_enter(
        server_seat,
        target_scene_surface->surface,
        sx,
        sy
      );
    }

    wlr_seat_pointer_notify_motion(server_seat, time_msec, sx, sy);
  }
}
