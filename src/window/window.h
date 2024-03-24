#ifndef CATNIP_WINDOW_H
#define CATNIP_WINDOW_H

#include "lua_resource.h"
#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  struct wl_list link;
  struct catnip_lua_resource* lua_resource;

  struct wlr_xdg_surface* xdg_surface;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;

  struct {
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener configure;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
    struct wl_listener destroy;
  } listeners;
};

extern struct wl_list catnip_windows;

void
catnip_window_init();

#endif
