#include "window_properties.h"
#include "config.h"
#include "lua_resource.h"
#include "utils/wayland.h"
#include <wlr/types/wlr_scene.h>

int
catnip_window_get_x(struct catnip_window* window)
{
  // NOTE: No need for `wlr_scene_node_coords` since our window scene nodes
  // already reside at the top level.
  return window->scene_tree->node.x;
}

void
catnip_window_set_x(struct catnip_window* window, int new_x)
{
  if (catnip_window_get_x(window) == new_x) {
    return;
  }

  wlr_scene_node_set_position(
    &window->scene_tree->node,
    new_x,
    catnip_window_get_y(window)
  );

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "property::x", 0);
}

int
catnip_window_get_y(struct catnip_window* window)
{
  // NOTE: No need for `wlr_scene_node_coords` since our window scene nodes
  // already reside at the top level.
  return window->scene_tree->node.y;
}

void
catnip_window_set_y(struct catnip_window* window, int new_y)
{
  if (catnip_window_get_y(window) == new_y) {
    return;
  }

  wlr_scene_node_set_position(
    &window->scene_tree->node,
    catnip_window_get_x(window),
    new_y
  );

  lua_catnip_resource_publish(catnip_L, window->lua_resource, "property::y", 0);
}

int
catnip_window_get_z(struct catnip_window* window)
{
  return wlr_scene_node_get_zindex(&window->scene_tree->node);
}

void
catnip_window_set_z(struct catnip_window* window, int new_z)
{
  if (catnip_window_get_z(window) == new_z) {
    return;
  }

  wlr_scene_node_set_zindex(&window->scene_tree->node, new_z);
  lua_catnip_resource_publish(catnip_L, window->lua_resource, "property::z", 0);
}

int
catnip_window_get_width(struct catnip_window* window)
{
  return window->latest.width;
}

void
catnip_window_set_width(struct catnip_window* window, int new_width)
{
  if (catnip_window_get_width(window) == new_width) {
    return;
  }

  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    new_width,
    catnip_window_get_height(window)
  );
  window->latest.width = new_width;

  lua_catnip_resource_publish(
    catnip_L,
    window->lua_resource,
    "property::width",
    0
  );
}

int
catnip_window_get_height(struct catnip_window* window)
{
  return window->latest.height;
}

void
catnip_window_set_height(struct catnip_window* window, int new_height)
{
  if (catnip_window_get_height(window) == new_height) {
    return;
  }

  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    catnip_window_get_width(window),
    new_height
  );
  window->latest.height = new_height;

  lua_catnip_resource_publish(
    catnip_L,
    window->lua_resource,
    "property::height",
    0
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
  if (catnip_window_get_visible(window) == new_visible) {
    return;
  }

  wlr_scene_node_set_enabled(&window->scene_tree->node, new_visible);

  lua_catnip_resource_publish(
    catnip_L,
    window->lua_resource,
    "property::visible",
    0
  );
}

bool
catnip_window_get_focused(struct catnip_window* window)
{
  return window->latest.focused;
}

void
catnip_window_set_focused(struct catnip_window* window, bool new_focused)
{
  if (catnip_window_get_focused(window) == new_focused) {
    return;
  }

  // NOTE: Since we sync the xdg_toplevel's active property with whether its
  // surface is the `catnip_seat`s focused keyboard surface, we "focus" the
  // window by scheduling an `activated` change in the next configure event.
  wlr_xdg_toplevel_set_activated(window->xdg_toplevel, new_focused);
  window->latest.focused = new_focused;

  lua_catnip_resource_publish(
    catnip_L,
    window->lua_resource,
    "property::focused",
    0
  );
}
