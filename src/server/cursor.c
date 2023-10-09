#include "cursor.h"
#include "server/output_layout.h"
#include "server/scene.h"
#include "server/seat.h"
#include "utils/wayland.h"
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xcursor_manager.h>

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

struct wlr_cursor* server_cursor;
enum server_cursor_mode server_cursor_mode;
static struct wlr_xcursor_manager* server_cursor_manager;

static struct {
  struct wl_listener cursor_motion;
  struct wl_listener cursor_motion_absolute;
  struct wl_listener cursor_button;
  struct wl_listener cursor_axis;
  struct wl_listener cursor_frame;
} listeners;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
update_server_cursor(uint32_t time_msec)
{
  double sx = 0;
  double sy = 0;

  struct wlr_scene_node* target_node = wlr_scene_node_at(
    &server_scene->tree.node,
    server_cursor->x,
    server_cursor->y,
    &sx,
    &sy
  );

  if (target_node == NULL || target_node->type != WLR_SCENE_NODE_BUFFER) {
    wlr_seat_pointer_clear_focus(server_seat);
    wlr_xcursor_manager_set_cursor_image(
      server_cursor_manager,
      "left_ptr",
      server_cursor
    );
  } else {
    struct wlr_surface* target_surface =
      wlr_scene_surface_from_buffer(wlr_scene_buffer_from_node(target_node))
        ->surface;

    wlr_seat_pointer_notify_enter(server_seat, target_surface, sx, sy);
    wlr_seat_pointer_notify_motion(server_seat, time_msec, sx, sy);
  }
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_cursor_motion(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    server_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  update_server_cursor(event->time_msec);
}

static void
on_cursor_motion_absolute(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    server_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  update_server_cursor(event->time_msec);
}

static void
on_cursor_button(struct wl_listener* listener, void* data)
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
on_cursor_axis(struct wl_listener* listener, void* data)
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
on_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(server_seat);
}

void
init_server_cursor()
{
  server_cursor_mode = SERVER_CURSOR_MODE_PASSTHROUGH;

  server_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(server_cursor, server_output_layout);

  server_cursor_manager = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(server_cursor_manager, 1);

  wl_setup_listener(
    &listeners.cursor_motion,
    &server_cursor->events.motion,
    on_cursor_motion
  );
  wl_setup_listener(
    &listeners.cursor_motion_absolute,
    &server_cursor->events.motion_absolute,
    on_cursor_motion_absolute
  );
  wl_setup_listener(
    &listeners.cursor_button,
    &server_cursor->events.button,
    on_cursor_button
  );
  wl_setup_listener(
    &listeners.cursor_axis,
    &server_cursor->events.axis,
    on_cursor_axis
  );
  wl_setup_listener(
    &listeners.cursor_frame,
    &server_cursor->events.frame,
    on_cursor_frame
  );
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

struct wlr_output*
server_cursor_get_output()
{
  return wlr_output_layout_output_at(
    server_output_layout,
    server_cursor->x,
    server_cursor->y
  );
}

int
server_cursor_get_lx()
{
  struct wlr_output* output = server_cursor_get_output();

  double lx = server_cursor->x;
  double ly = server_cursor->y;
  wlr_output_layout_output_coords(server_output_layout, output, &lx, &ly);

  return lx;
}

int
server_cursor_get_ly()
{
  struct wlr_output* output = server_cursor_get_output();

  double lx = server_cursor->x;
  double ly = server_cursor->y;
  wlr_output_layout_output_coords(server_output_layout, output, &lx, &ly);

  return ly;
}

int
server_cursor_get_gx()
{
  return server_cursor->x;
}

int
server_cursor_get_gy()
{
  return server_cursor->y;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_cursor_set_lx(int new_lx)
{
  struct wlr_output* output = server_cursor_get_output();
  struct wlr_output_layout_output* layout =
    wlr_output_layout_get(server_output_layout, output);

  wlr_cursor_warp_closest(
    server_cursor,
    NULL,
    layout->x + new_lx,
    server_cursor_get_gy()
  );

  update_server_cursor(wlr_get_time_msec());
}

void
server_cursor_set_ly(int new_ly)
{
  struct wlr_output* output = server_cursor_get_output();
  struct wlr_output_layout_output* layout =
    wlr_output_layout_get(server_output_layout, output);

  wlr_cursor_warp_closest(
    server_cursor,
    NULL,
    server_cursor_get_gx(),
    layout->y + new_ly
  );

  update_server_cursor(wlr_get_time_msec());
}

void
server_cursor_set_gx(int new_gx)
{
  wlr_cursor_warp_closest(server_cursor, NULL, new_gx, server_cursor_get_gy());
  update_server_cursor(wlr_get_time_msec());
}

void
server_cursor_set_gy(int new_gy)
{
  wlr_cursor_warp_closest(server_cursor, NULL, server_cursor_get_gx(), new_gy);
  update_server_cursor(wlr_get_time_msec());
}
