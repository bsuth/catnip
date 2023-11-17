#ifndef CATNIP_WINDOW_H
#define CATNIP_WINDOW_H

#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  struct wl_list link;

  struct wlr_xdg_surface* xdg_surface;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;

  struct catnip_window** lua_userdata;

  // These properties cannot be updated immediately and must instead _request_
  // an update. Here, we store the most recent values requested.
  struct {
    int width;
    int height;
  } pending;

  struct {
    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
  } listeners;
};

extern struct wl_list catnip_windows;

void
catnip_window_init();

#endif
