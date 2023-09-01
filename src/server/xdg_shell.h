#ifndef SERVER_XDG_SHELL_H
#define SERVER_XDG_SHELL_H

#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>

// TODO: remove this?
struct server_view
{
  int x, y;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;
  struct wl_list link;
  struct wl_listener map;
  struct wl_listener unmap;
  struct wl_listener destroy;
  struct wl_listener request_move;
  struct wl_listener request_resize;
  struct wl_listener request_maximize;
  struct wl_listener request_fullscreen;
};

extern struct wlr_xdg_shell* server_xdg_shell;
extern struct wl_list server_views;

void
server_xdg_shell_init();

#endif
