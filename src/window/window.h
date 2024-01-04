#ifndef CATNIP_WINDOW_H
#define CATNIP_WINDOW_H

#include "utils/lua.h"
#include <wlr/types/wlr_xdg_shell.h>

struct catnip_window {
  int id;
  struct wl_list link;

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

  struct {
    struct catnip_window** userdata;
    lua_Ref ref;
    lua_Ref subscriptions;
  } lua;

  // Since configure events do not tell us which fields have actually changed,
  // we store the most recent configure event values here and do the diff
  // ourselves.
  //
  // Note that `struct wlr_xdg_toplevel_configure` has a `fields` property, but
  // at the time of writing it only tells us if `bounds` or `wm_capabilities`
  // has changed.
  struct {
    bool activated;
  } prev_configure;

  // These properties cannot be updated immediately and must instead _request_
  // an update. Here, we store the most recent values requested.
  struct {
    int width;
    int height;
  } pending;
};

extern struct wl_list catnip_windows;

void
catnip_window_init();

#endif
