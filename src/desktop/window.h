#ifndef CATNIP_DESKTOP_WINDOW_H
#define CATNIP_DESKTOP_WINDOW_H

#include "lua_resource.h"
#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  int id;

  struct wl_list link;
  struct catnip_lua_resource* lua_resource;

  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;

  struct wl_listener map_listener;
  struct wl_listener commit_listener;
  struct wl_listener unmap_listener;
  struct wl_listener request_maximize_listener;
  struct wl_listener request_fullscreen_listener;
  struct wl_listener destroy_listener;
};

struct catnip_window*
catnip_window_create(struct wlr_xdg_toplevel* xdg_toplevel);

void
catnip_window_focus(struct catnip_window* window);

#endif
