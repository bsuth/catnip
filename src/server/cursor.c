#include "cursor.h"
#include "output.h"
#include "seat.h"
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_xcursor_manager.h>

struct wlr_cursor* server_cursor;
enum server_cursor_mode server_cursor_mode;
static struct wlr_xcursor_manager* server_cursor_manager;

static struct wl_listener cursor_motion_listener;
static struct wl_listener cursor_motion_absolute_listener;
static struct wl_listener cursor_button_listener;
static struct wl_listener cursor_axis_listener;
static struct wl_listener cursor_frame_listener;

static void
reset_cursor_mode()
{
  /* Reset the cursor mode to passthrough. */
  server_cursor_mode = SERVER_CURSOR_MODE_PASSTHROUGH;
  /* server->grabbed_view = NULL; */
}

static void
process_cursor_move(uint32_t time)
{
  // TODO
  /* struct server_view* view = grabbed_view; */

  /* view->x = server_cursor->x - grab_x; */
  /* view->y = server_cursor->y - grab_y; */

  /* wlr_scene_node_set_position(&view->scene_tree->node, view->x, view->y); */
}

static void
process_cursor_resize(uint32_t time)
{
  // TODO
  /*
   * Resizing the grabbed view can be a little bit complicated, because we
   * could be resizing from any corner or edge. This not only resizes the view
   * on one or two axes, but can also move the view if you resize from the top
   * or left edges (or top-left corner).
   *
   * Note that I took some shortcuts here. In a more fleshed-out compositor,
   * you'd wait for the client to prepare a buffer at the new size, then
   * commit any movement that was prepared.
   */
  /* struct server_view* view = grabbed_view; */
  /* double border_x = server_cursor->x - server->grab_x; */
  /* double border_y = server_cursor->y - server->grab_y; */
  /* int new_left = server->grab_geobox.x; */
  /* int new_right = server->grab_geobox.x + server->grab_geobox.width; */
  /* int new_top = server->grab_geobox.y; */
  /* int new_bottom = server->grab_geobox.y + server->grab_geobox.height; */

  /* if (server->resize_edges & WLR_EDGE_TOP) { */
  /*   new_top = border_y; */
  /*   if (new_top >= new_bottom) { */
  /*     new_top = new_bottom - 1; */
  /*   } */
  /* } else if (server->resize_edges & WLR_EDGE_BOTTOM) { */
  /*   new_bottom = border_y; */
  /*   if (new_bottom <= new_top) { */
  /*     new_bottom = new_top + 1; */
  /*   } */
  /* } */
  /* if (server->resize_edges & WLR_EDGE_LEFT) { */
  /*   new_left = border_x; */
  /*   if (new_left >= new_right) { */
  /*     new_left = new_right - 1; */
  /*   } */
  /* } else if (server->resize_edges & WLR_EDGE_RIGHT) { */
  /*   new_right = border_x; */
  /*   if (new_right <= new_left) { */
  /*     new_right = new_left + 1; */
  /*   } */
  /* } */

  /* struct wlr_box geo_box; */
  /* wlr_xdg_surface_get_geometry(view->xdg_toplevel->base, &geo_box); */
  /* view->x = new_left - geo_box.x; */
  /* view->y = new_top - geo_box.y; */
  /* wlr_scene_node_set_position(&view->scene_tree->node, view->x, view->y); */

  /* int new_width = new_right - new_left; */
  /* int new_height = new_bottom - new_top; */
  /* wlr_xdg_toplevel_set_size(view->xdg_toplevel, new_width, new_height); */
}

static void
process_cursor_motion(uint32_t time)
{
  /* If the mode is non-passthrough, delegate to those functions. */
  if (server_cursor_mode == SERVER_CURSOR_MODE_MOVE) {
    process_cursor_move(time);
    return;
  } else if (server_cursor_mode == SERVER_CURSOR_MODE_RESIZE) {
    process_cursor_resize(time);
    return;
  }

  /* Otherwise, find the view under the pointer and send the event along. */
  double sx, sy;
  struct wlr_seat* seat = server_seat;
  struct wlr_surface* surface = NULL;
  /* struct server_view* view = */
  /*   desktop_view_at(server, server_cursor->x, server_cursor->y, &surface,
   * &sx, &sy); */
  /* if (!view) { */
  /*   /1* If there's no view under the cursor, set the cursor image to a */
  /*    * default. This is what makes the cursor image appear when you move it
   */
  /*    * around the screen, not over any views. *1/ */
  /*   wlr_xcursor_manager_set_cursor_image( */
  /*     server_cursor_manager, "left_ptr", server_cursor); */
  /* } */
  if (surface) {
    /*
     * Send pointer enter and motion events.
     *
     * The enter event gives the surface "pointer focus", which is distinct
     * from keyboard focus. You get pointer focus by moving the pointer over
     * a window.
     *
     * Note that wlroots will avoid sending duplicate enter/motion events if
     * the surface has already has pointer focus or if the client is already
     * aware of the coordinates passed.
     */
    wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
    wlr_seat_pointer_notify_motion(seat, time, sx, sy);
  } else {
    wlr_xcursor_manager_set_cursor_image(
      server_cursor_manager,
      "left_ptr",
      server_cursor
    );

    /* Clear pointer focus so future button events and such are not sent to
     * the last client to have the cursor over it. */
    wlr_seat_pointer_clear_focus(seat);
  }
}

static void
cursor_motion_notify(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    server_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  process_cursor_motion(event->time_msec);
}

static void
cursor_motion_absolute_notify(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    server_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  process_cursor_motion(event->time_msec);
}

static void
cursor_button_notify(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  wlr_seat_pointer_notify_button(
    server_seat,
    event->time_msec,
    event->button,
    event->state
  );

  if (event->state == WLR_BUTTON_RELEASED) {
    reset_cursor_mode();
  } else {
    /* double sx, sy; */
    /* struct wlr_surface* surface = NULL; */
    /* struct server_view* view = */
    /*   desktop_view_at(server, server_cursor->x, server_cursor->y, &surface,
     * &sx, &sy); */
    /* focus_view(view, surface); */
  }
}

static void
cursor_axis_notify(struct wl_listener* listener, void* data)
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
cursor_frame_notify(struct wl_listener* listener, void* data)
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

  cursor_motion_listener.notify = cursor_motion_notify;
  wl_signal_add(&server_cursor->events.motion, &cursor_motion_listener);
  cursor_motion_absolute_listener.notify = cursor_motion_absolute_notify;
  wl_signal_add(
    &server_cursor->events.motion_absolute,
    &cursor_motion_absolute_listener
  );
  cursor_button_listener.notify = cursor_button_notify;
  wl_signal_add(&server_cursor->events.button, &cursor_button_listener);
  cursor_axis_listener.notify = cursor_axis_notify;
  wl_signal_add(&server_cursor->events.axis, &cursor_axis_listener);
  cursor_frame_listener.notify = cursor_frame_notify;
  wl_signal_add(&server_cursor->events.frame, &cursor_frame_listener);
}
