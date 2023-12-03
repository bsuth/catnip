#include "window_properties.h"
#include "input/seat.h"
#include <wlr/types/wlr_scene.h>

int
catnip_window_get_x(struct catnip_window* window)
{
  int x, y;
  wlr_scene_node_coords(&window->scene_tree->node, &x, &y);
  return x;
}

void
catnip_window_set_x(struct catnip_window* window, int new_x)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    new_x,
    catnip_window_get_y(window)
  );
}

int
catnip_window_get_y(struct catnip_window* window)
{
  int x, y;
  wlr_scene_node_coords(&window->scene_tree->node, &x, &y);
  return y;
}

void
catnip_window_set_y(struct catnip_window* window, int new_y)
{
  wlr_scene_node_set_position(
    &window->scene_tree->node,
    catnip_window_get_x(window),
    new_y
  );
}

int
catnip_window_get_width(struct catnip_window* window)
{
  return (window->pending.width != -1)
           ? window->pending.width
           : window->xdg_surface->surface->current.width;
}

void
catnip_window_set_width(struct catnip_window* window, int new_width)
{
  window->pending.width = new_width;

  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    new_width,
    catnip_window_get_height(window)
  );
}

int
catnip_window_get_height(struct catnip_window* window)
{
  return (window->pending.height != -1)
           ? window->pending.height
           : window->xdg_surface->surface->current.height;
}

void
catnip_window_set_height(struct catnip_window* window, int new_height)
{
  window->pending.height = new_height;

  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    catnip_window_get_width(window),
    new_height
  );
}

bool
catnip_window_get_focused(struct catnip_window* window)
{
  return window->xdg_toplevel->base->surface
         == catnip_seat->keyboard_state.focused_surface;
}

void
catnip_window_set_focused(struct catnip_window* window, bool new_focused)
{
  if (catnip_window_get_focused(window) == new_focused) {
    return; // nothing to do
  } else if (new_focused == false) {
    wlr_xdg_toplevel_set_activated(window->xdg_toplevel, false);
    wlr_seat_keyboard_notify_clear_focus(catnip_seat);
  } else {
    wlr_scene_node_raise_to_top(&window->scene_tree->node);
    wlr_xdg_toplevel_set_activated(window->xdg_toplevel, true);

    struct wlr_keyboard* keyboard = wlr_seat_get_keyboard(catnip_seat);

    if (keyboard != NULL) {
      wlr_seat_keyboard_notify_enter(
        catnip_seat,
        window->xdg_toplevel->base->surface,
        keyboard->keycodes,
        keyboard->num_keycodes,
        &keyboard->modifiers
      );
    }
  }
}

bool
catnip_window_get_maximized(struct catnip_window* window)
{
  // TODO
  return false;
}

void
catnip_window_set_maximized(struct catnip_window* window, bool new_maximized)
{
  // TODO
}

bool
catnip_window_get_fullscreen(struct catnip_window* window)
{
  // TODO
  return false;
}

void
catnip_window_set_fullscreen(struct catnip_window* window, bool new_fullscreen)
{
  // TODO
}
