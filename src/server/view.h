#ifndef DESKTOP_VIEW_H
#define DESKTOP_VIEW_H

#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>

struct desktop_view {
  int x, y;
  struct wlr_xdg_toplevel* xdg_toplevel;
  struct wlr_scene_tree* scene_tree;
};

#endif
