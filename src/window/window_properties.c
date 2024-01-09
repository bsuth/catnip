#include "window_properties.h"
#include "utils/wayland.h"
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
catnip_window_get_z(struct catnip_window* window)
{
  return wlr_scene_node_get_zindex(&window->scene_tree->node);
}

void
catnip_window_set_z(struct catnip_window* window, int new_z)
{
  wlr_scene_node_set_zindex(&window->scene_tree->node, new_z);
}

int
catnip_window_get_width(struct catnip_window* window)
{
  return window->xdg_toplevel->scheduled.width;
}

void
catnip_window_set_width(struct catnip_window* window, int new_width)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    new_width,
    catnip_window_get_height(window)
  );
}

int
catnip_window_get_height(struct catnip_window* window)
{
  return window->xdg_toplevel->scheduled.height;
}

void
catnip_window_set_height(struct catnip_window* window, int new_height)
{
  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    catnip_window_get_width(window),
    new_height
  );
}

bool
catnip_window_get_visible(struct catnip_window* window)
{
  return window->scene_tree->node.enabled;
}

void
catnip_window_set_visible(struct catnip_window* window, bool new_visible)
{
  wlr_scene_node_set_enabled(&window->scene_tree->node, new_visible);
}

bool
catnip_window_get_focused(struct catnip_window* window)
{
  // Since we sync the xdg_toplevel's active property with whether its surface
  // is the keyboard focused surface for `catnip_seat`, we just get the
  // `active` property here for simplicity and consistency.
  return window->xdg_toplevel->scheduled.activated;
}

void
catnip_window_set_focused(struct catnip_window* window, bool new_focused)
{
  // Since we sync the xdg_toplevel's active property with whether its surface
  // is the keyboard focused surface for `catnip_seat`, we just set the
  // `active` property here for simplicity and consistency.
  wlr_xdg_toplevel_set_activated(window->xdg_toplevel, new_focused);
}

bool
catnip_window_get_fullscreen(struct catnip_window* window)
{
  return window->xdg_toplevel->scheduled.fullscreen;
}

void
catnip_window_set_fullscreen(struct catnip_window* window, bool new_fullscreen)
{
  wlr_xdg_toplevel_set_fullscreen(window->xdg_toplevel, new_fullscreen);
}

bool
catnip_window_get_maximized(struct catnip_window* window)
{
  return window->xdg_toplevel->scheduled.maximized;
}

void
catnip_window_set_maximized(struct catnip_window* window, bool new_maximized)
{
  wlr_xdg_toplevel_set_maximized(window->xdg_toplevel, new_maximized);
}
