#include "cursor.h"
#include "output.h"
#include "seat.h"
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_xcursor_manager.h>

struct wlr_cursor* bwc_cursor;
enum bwc_cursor_mode bwc_cursor_mode;
struct wlr_xcursor_manager* bwc_cursor_mgr;

struct wl_listener cursor_motion_listener;
struct wl_listener cursor_motion_absolute_listener;
struct wl_listener cursor_button_listener;
struct wl_listener cursor_axis_listener;
struct wl_listener cursor_frame_listener;

static void
reset_cursor_mode()
{
  /* Reset the cursor mode to passthrough. */
  bwc_cursor_mode = BWC_CURSOR_PASSTHROUGH;
  /* server->grabbed_view = NULL; */
}

static void
process_cursor_move(uint32_t time)
{
  // TODO
  /* struct bwc_view* view = grabbed_view; */

  /* view->x = bwc_cursor->x - grab_x; */
  /* view->y = bwc_cursor->y - grab_y; */

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
  /* struct bwc_view* view = grabbed_view; */
  /* double border_x = bwc_cursor->x - server->grab_x; */
  /* double border_y = bwc_cursor->y - server->grab_y; */
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
  if (bwc_cursor_mode == BWC_CURSOR_MOVE) {
    process_cursor_move(time);
    return;
  } else if (bwc_cursor_mode == BWC_CURSOR_RESIZE) {
    process_cursor_resize(time);
    return;
  }

  /* Otherwise, find the view under the pointer and send the event along. */
  double sx, sy;
  struct wlr_seat* seat = bwc_seat;
  struct wlr_surface* surface = NULL;
  /* struct bwc_view* view = */
  /*   desktop_view_at(server, bwc_cursor->x, bwc_cursor->y, &surface, &sx,
   * &sy); */
  /* if (!view) { */
  /*   /1* If there's no view under the cursor, set the cursor image to a */
  /*    * default. This is what makes the cursor image appear when you move it
   */
  /*    * around the screen, not over any views. *1/ */
  /*   wlr_xcursor_manager_set_cursor_image( */
  /*     bwc_cursor_mgr, "left_ptr", bwc_cursor); */
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
    bwc_cursor, &event->pointer->base, event->delta_x, event->delta_y);

  process_cursor_motion(event->time_msec);
}

static void
cursor_motion_absolute_notify(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    bwc_cursor, &event->pointer->base, event->x, event->y);

  process_cursor_motion(event->time_msec);
}

static void
cursor_button_notify(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  /* Notify the client with pointer focus that a button press has occurred */
  wlr_seat_pointer_notify_button(
    bwc_seat, event->time_msec, event->button, event->state);

  double sx, sy;
  struct wlr_surface* surface = NULL;
  /* struct bwc_view* view = */
  /*   desktop_view_at(server, bwc_cursor->x, bwc_cursor->y, &surface, &sx,
   * &sy); */

  if (event->state == WLR_BUTTON_RELEASED) {
    /* If you released any buttons, we exit interactive move/resize mode. */
    reset_cursor_mode();
  } else {
    /* Focus that client if the button was _pressed_ */
    /* focus_view(view, surface); */
  }
}

static void
cursor_axis_notify(struct wl_listener* listener, void* data)
{
  /* This event is forwarded by the cursor when a pointer emits an axis event,
   * for example when you move the scroll wheel. */
  struct wlr_pointer_axis_event* event = data;

  /* Notify the client with pointer focus of the axis event. */
  wlr_seat_pointer_notify_axis(bwc_seat,
                               event->time_msec,
                               event->orientation,
                               event->delta,
                               event->delta_discrete,
                               event->source);
}

static void
cursor_frame_notify(struct wl_listener* listener, void* data)
{
  /* This event is forwarded by the cursor when a pointer emits an frame
   * event. Frame events are sent after regular pointer events to group
   * multiple events together. For instance, two axis events may happen at the
   * same time, in which case a frame event won't be sent in between. */

  /* Notify the client with pointer focus of the frame event. */
  wlr_seat_pointer_notify_frame(bwc_seat);
}

void
bwc_cursor_init()
{
  bwc_cursor = wlr_cursor_create();
  wlr_cursor_attach_output_layout(bwc_cursor, bwc_output_layout);

  bwc_cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
  wlr_xcursor_manager_load(bwc_cursor_mgr, 1);

  bwc_cursor_mode = BWC_CURSOR_PASSTHROUGH;

  cursor_motion_listener.notify = cursor_motion_notify;
  wl_signal_add(&bwc_cursor->events.motion, &cursor_motion_listener);
  cursor_motion_absolute_listener.notify = cursor_motion_absolute_notify;
  wl_signal_add(&bwc_cursor->events.motion_absolute,
                &cursor_motion_absolute_listener);
  cursor_button_listener.notify = cursor_button_notify;
  wl_signal_add(&bwc_cursor->events.button, &cursor_button_listener);
  cursor_axis_listener.notify = cursor_axis_notify;
  wl_signal_add(&bwc_cursor->events.axis, &cursor_axis_listener);
  cursor_frame_listener.notify = cursor_frame_notify;
  wl_signal_add(&bwc_cursor->events.frame, &cursor_frame_listener);
}
