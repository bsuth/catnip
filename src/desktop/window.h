#ifndef CATNIP_DESKTOP_WINDOW_H
#define CATNIP_DESKTOP_WINDOW_H

#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  int id;
  struct wl_list link;
  struct catnip_lua_window* lua_window;

  struct {
    struct wlr_xdg_toplevel* xdg_toplevel;
    struct wlr_scene_tree* scene_tree;
  } wlr;

  struct {
    struct wl_listener surface_map;
    struct wl_listener surface_commit;
    struct wl_listener surface_unmap;
    struct wl_listener xdg_toplevel_request_maximize;
    struct wl_listener xdg_toplevel_request_fullscreen;
    struct wl_listener xdg_toplevel_destroy;
  } listeners;
};

struct catnip_window*
catnip_window_create(struct wlr_xdg_toplevel* xdg_toplevel);

void
catnip_window_focus(struct catnip_window* window);

#endif
